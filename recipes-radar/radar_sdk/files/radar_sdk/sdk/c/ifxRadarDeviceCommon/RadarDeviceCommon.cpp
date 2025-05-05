/* ===========================================================================
** Copyright (C) 2022 Infineon Technologies AG
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
** ===========================================================================
*/

#include <cstring>
#include <functional>
#include <mutex>
#include <vector>

#include "RadarDeviceCommon.h"
#include "internal/RadarDeviceCommon.hpp"
#include "internal/RfShieldEeprom.hpp"

#include <platform/BoardManager.hpp>
#include <platform/exception/EConnection.hpp>

#include <components/interfaces/IRadar.hpp>
#include <components/interfaces/IRadarAvian.hpp>
#include <components/interfaces/IRadarAtr22.hpp>
#include <components/interfaces/IRadarLtr11.hpp>
#include <components/nonvolatileMemory/NonvolatileMemoryEepromI2c.hpp>

/* We use the function Infineon::Avian::detect_device_type to determine
 * what kind of radar sensor of the Avian family is used. This, unfortunately,
 * adds a dependency to Avian. But copying the logic from Avian would be even
 * worse.
 * If in case we need to break this dependency in the future, we can add #ifdefs.
 */
#include <ifxAvian_Utilities.hpp>

namespace
{
    constexpr bool use_serial = true;   // search for boards connected via USB
    constexpr bool use_ethernet = true; // search for boards connected via ethernet

    /* board_manager may be used by multiple threads concurrently. Each access to
     * board_manager must be protected using the mutex mutex_board_manager. This
     * presents "weird" behavior like that some boards are not found if you
     * enumerate the boards from two different threads at the same time.
     */
    std::mutex mutex_board_manager;

    /**
     * @brief Determine the radar sensor
     *
     * Find out to which radar sensor we are currently connected.
     *
     * @param [in]  board           board instance
     * @param [out] sensor_type     type of radar sensor
     * @retval  true                if successful
     * @retval  false               if unknown (i.e., unsupported) radar sensor
     */
    bool get_sensor_type(std::unique_ptr<BoardInstance>& board, ifx_Radar_Sensor_t& sensor_type)
    {
        auto componentImplementation = board->getComponentImplementation<IRadar>(0);

        if (componentImplementation == IRadarAvian::getImplementation())
        {
            auto* component = board->getComponent<IRadarAvian>(0);

            // fix the SPI high-speed compensation, so that we can read the
            // chip id register reliable
            component->getIRegisters()->write(0x06, 0x100000);

            uint32_t chip_id_register = component->getIRegisters()->read(0x02);
            sensor_type = ifx_Radar_Sensor_t(Infineon::Avian::detect_device_type(chip_id_register));
            return true;
        }
        else if (componentImplementation == IRadarAtr22::getImplementation())
        {
            sensor_type = IFX_MIMOSE_BGT24ATR22;
            return true;
        }
        else if (componentImplementation == IRadarLtr11::getImplementation())
        {
            sensor_type = IFX_BGT60LTR11AIP;
            return true;
        }

        return false;
    }

    std::vector<ifx_Radar_Sensor_List_Entry_t> get_list(BoardManager& board_manager, const rdk::RadarDeviceCommon::SelectorFunction& selector)
    {
        std::vector<ifx_Radar_Sensor_List_Entry_t> list;

        for (const auto& descriptor : board_manager.getEnumeratedList())
        {
            ifx_Radar_Sensor_List_Entry_t entry = {};

            try
            {
                std::unique_ptr<BoardInstance> board = descriptor->createBoardInstance();

                if (!get_sensor_type(board, entry.sensor_type))
                {
                    // not a radar sensor that we support
                    continue;
                }

                entry.board_type = rdk::RadarDeviceCommon::get_boardtype_from_pid(board->getPid());

                // read uuid
                const auto uuid= board->getUuid();
                std::copy(uuid.begin(), uuid.end(), entry.uuid);

                if (selector(entry))
                    list.push_back(entry);
            }
            catch (const EException&)
            {
                continue;
            }
        }

        return list;
    }
}

using namespace rdk::RadarDeviceCommon;

std::unique_ptr<BoardInstance> rdk::RadarDeviceCommon::open_by_uuid(const uint8_t uuid[16])
{
    std::unique_lock<std::mutex> lock(mutex_board_manager);

    BoardManager board_manager(use_serial, use_ethernet);
    board_manager.enumerate();

    try
    {
        return board_manager.createSpecificBoardInstance(uuid);
    }
    catch (EException&)
    {
    }

    return nullptr;
}

std::vector<ifx_Radar_Sensor_List_Entry_t> rdk::RadarDeviceCommon::get_list(const SelectorFunction& selector)
{
    std::unique_lock<std::mutex> lock(mutex_board_manager);

    BoardManager board_manager(use_serial, use_ethernet);
    board_manager.enumerate();

    return ::get_list(board_manager, selector);
}

std::unique_ptr<BoardInstance> rdk::RadarDeviceCommon::open(const SelectorFunction& selector)
{
    std::unique_lock<std::mutex> lock(mutex_board_manager);

    BoardManager board_manager(use_serial, use_ethernet);
    board_manager.enumerate();

    auto list = ::get_list(board_manager, selector);
    if (list.empty())
        return nullptr;

    try
    {
        return board_manager.createSpecificBoardInstance(list[0].uuid);
    }
    catch (EException&)
    {
    }

    return nullptr;
}

void rdk::RadarDeviceCommon::get_firmware_info(BoardInstance* board, ifx_Firmware_Info_t* firmware_info)
{
    const auto version = board->getIBridge()->getIBridgeControl()->getVersionInfo();
    firmware_info->description = board->getName();
    firmware_info->version_major = version[0];
    firmware_info->version_minor = version[1];
    firmware_info->version_build = version[2];
    firmware_info->extendedVersion = board->getIBridge()->getIBridgeControl()->getExtendedVersionString().c_str();
}

bool rdk::RadarDeviceCommon::is_firmware_version_valid(const std::array<uint16_t, 3> &min_version, ifx_Firmware_Info_t firmware_info)
{
    const std::array<uint16_t, 3> version{firmware_info.version_major, firmware_info.version_minor, firmware_info.version_build};
    return !std::lexicographical_compare(version.begin(), version.end(), min_version.begin(), min_version.end());
}

ifx_Error_e rdk::RadarDeviceCommon::get_shield_info(BoardInstance* board, ifx_RF_Shield_Info_t *shield_info)
{
    constexpr auto i2cBusId = 0;  // shield is always mapped to default bus id
    auto *i2c = board->getIBridge()->getIBridgeControl()->getII2c();
    auto eeprom = RfShieldEeprom(i2c, i2cBusId);

    ifx_RF_Shield_Eeprom_Header_t header = {};
    ifx_Error_e errors = eeprom.readHeader(&header);
    if (errors)
    {
        return errors;
    }

    shield_info->type = static_cast<ifx_RF_Shield_Type_t>(header.shield_type);
    return IFX_OK;
}