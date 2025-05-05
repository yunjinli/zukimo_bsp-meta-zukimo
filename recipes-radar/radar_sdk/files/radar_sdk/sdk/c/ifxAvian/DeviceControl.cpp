/* ===========================================================================
** Copyright (C) 2021 - 2022 Infineon Technologies AG
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

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
*/

#include <array>
#include <atomic>
#include <chrono>
#include <cctype>
#include <condition_variable>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <memory>
#include <string>
#include <vector>

#include "ifxBase/Defines.h"
#include "ifxBase/Error.h"
#include "ifxBase/Exception.hpp"
#include "ifxBase/internal/Util.h"
#include "ifxBase/List.h"
#include "ifxBase/internal/List.hpp"
#include "ifxBase/Log.h"
#include "ifxBase/Mem.h"
#include "ifxBase/Matrix.h"
#include "ifxBase/Uuid.h"

#include "ifxAvian/internal/DeviceCalc.h"
#include "ifxAvian/internal/RadarDevice.hpp"
#include "ifxAvian/internal/DummyRadarDevice.hpp"
#include "ifxAvian/internal/RecordingRadarDevice.hpp"
#include "ifxAvian/DeviceControl.h"
#include "ifxAvian/DeviceConfig.h"
#include "ifxAvian/Shapes.h"

#include "ifxRadarDeviceCommon/internal/RadarDeviceCommon.hpp"

// libAvian
#include <ifxAvian_Driver.hpp>

// Strata
#include <common/Logger.hpp>
#include <platform/BoardManager.hpp>
#include <platform/serial/BoardSerial.hpp>
#include <platform/exception/EConnection.hpp>
#include <platform/exception/EAlreadyOpened.hpp>
#include <platform/exception/EProtocol.hpp>

// DeviceControlHelper functions
#include "DeviceControlHelper.hpp"

using namespace Infineon;

/*
==============================================================================
   2. LOCAL DEFINITIONS
==============================================================================
*/

// Log level for Strata.
#ifdef NDEBUG
    // Release build: set log level to ERROR only
    #define STRATA_LOG_LEVEL (LOG_ERROR)
#else
    // Debug build: Set log level to error, warn, debug
    #define STRATA_LOG_LEVEL (LOG_ERROR | LOG_WARN | LOG_DEBUG)
#endif

/*
==============================================================================
   3. LOCAL TYPES
==============================================================================
*/

/*
==============================================================================
   5. LOCAL FUNCTION PROTOTYPES
==============================================================================
*/

/*
==============================================================================
   6. LOCAL FUNCTIONS
==============================================================================
*/

static ifx_Avian_Device_t* open_dummy_device(ifx_Radar_Sensor_t& sensor_type)
{
    return new ifx_Dummy_Radar_Device_s(sensor_type);
}

ifx_Avian_Device_t* open_device(std::unique_ptr<BoardInstance> board)
{
    return new ifx_Radar_Device_s(std::move(board));
}

/*
==============================================================================
   7. EXPORTED FUNCTIONS
==============================================================================
*/

ifx_List_t* ifx_avian_get_list_by_sensor_type(ifx_Radar_Sensor_t sensor_type)
{
    auto selector = [&sensor_type](const ifx_Radar_Sensor_List_Entry_t& entry) {
        return entry.sensor_type == sensor_type;
    };

    auto list = rdk::RadarDeviceCommon::get_list(selector);

    return ifx_list_from_vector(list);
}

//----------------------------------------------------------------------------

ifx_List_t* ifx_avian_get_list()
{
    auto selector = [](const ifx_Radar_Sensor_List_Entry_t& entry) {
        return rdk::RadarDeviceCommon::sensor_is_avian(entry.sensor_type);
    };

    auto list = rdk::RadarDeviceCommon::get_list(selector);
    return ifx_list_from_vector(list);
}

//----------------------------------------------------------------------------

ifx_Avian_Device_t* ifx_avian_create_by_port(const char* port)
{
    // if port is NULL, call ifx_avian_create. This ensures the previous behavior.
    if (port == nullptr)
        return ifx_avian_create();

    auto open_device_from_port = [&port]() {
        auto board = BoardSerial::createBoardInstance(port);
        return open_device(std::move(board));
    };

    return rdk::RadarDeviceCommon::exec_func<ifx_Avian_Device_t*>(open_device_from_port, nullptr);
}

//----------------------------------------------------------------------------

ifx_Avian_Device_t* ifx_avian_create_dummy(ifx_Radar_Sensor_t sensor_type)
{
    auto open_dummy_device_of_sensor_type = [&sensor_type]() {
        return open_dummy_device(sensor_type);
    };

    return rdk::RadarDeviceCommon::exec_func<ifx_Avian_Device_t*>(open_dummy_device_of_sensor_type, nullptr);
}

//----------------------------------------------------------------------------

ifx_Avian_Device_t* ifx_avian_create_dummy_from_recording(ifx_Recording_t* recording, bool correct_timing)
{
    auto open_dummy_device_of_sensor_type = [&recording, &correct_timing]() {
        return new RecordingRadarDevice(recording, correct_timing);
    };

    return rdk::RadarDeviceCommon::exec_func<ifx_Avian_Device_t*>(open_dummy_device_of_sensor_type, nullptr);
}

ifx_Avian_Device_t* ifx_avian_create()
{
    auto open_device_from_selector = []() {
        auto selector = [](const ifx_Radar_Sensor_List_Entry_t& entry) {
            return rdk::RadarDeviceCommon::sensor_is_avian(entry.sensor_type);
        };
        auto board = rdk::RadarDeviceCommon::open(selector);
        return open_device(std::move(board));
    };

    return rdk::RadarDeviceCommon::exec_func<ifx_Avian_Device_t*>(open_device_from_selector, nullptr);
}

//----------------------------------------------------------------------------

ifx_Avian_Device_t* ifx_avian_create_by_uuid(const char* uuid)
{
    IFX_ERR_BRV_NULL(uuid, nullptr);

    uint8_t uuid_array[16];
    if (!ifx_uuid_from_string(uuid, uuid_array))
    {
        ifx_error_set(IFX_ERROR_ARGUMENT_INVALID);
        return nullptr;
    }

    auto open_device_by_uuid = [&uuid_array]() {
        auto board = rdk::RadarDeviceCommon::open_by_uuid(uuid_array);
        return open_device(std::move(board));
    };
    
    return rdk::RadarDeviceCommon::exec_func<ifx_Avian_Device_t*>(open_device_by_uuid, nullptr);
}

//----------------------------------------------------------------------------

char* ifx_avian_get_register_list_string(ifx_Avian_Device_t* handle, bool set_trigger_bit)
{
	IFX_ERR_BRV_NULL(handle, nullptr);

    auto get_register_list = [&handle, &set_trigger_bit]() {
		return handle->get_register_list_string(set_trigger_bit);
	};

    return rdk::RadarDeviceCommon::exec_func<char*>(get_register_list, {});
}

//----------------------------------------------------------------------------

ifx_Radar_Sensor_t ifx_avian_get_sensor_type(const ifx_Avian_Device_t* handle)
{
    IFX_ERR_BRV_NULL(handle, IFX_RADAR_SENSOR_UNKNOWN);
    return handle->get_sensor_type();
}

//----------------------------------------------------------------------------

Infineon::Avian::Driver* ifx_avian_get_avian_driver(const ifx_Avian_Device_t* handle)
{
    IFX_ERR_BRV_NULL(handle, nullptr);
    return handle->get_avian_driver();
}

//----------------------------------------------------------------------------
BoardInstance* ifx_avian_get_strata_avian_board(const ifx_Avian_Device_t* handle)
{
    IFX_ERR_BRV_NULL(handle, nullptr);
    return handle->get_strata_avian_board();
}

//----------------------------------------------------------------------------

Infineon::Avian::StrataPort* ifx_avian_get_strata_avian_port(const ifx_Avian_Device_t* handle)
{
    IFX_ERR_BRV_NULL(handle, nullptr);
    return handle->get_strata_avian_port();
}

Infineon::Avian::Constant_Wave_Controller* ifx_avian_get_constant_wave_controller(ifx_Avian_Device_t* handle)
{
    IFX_ERR_BRV_NULL(handle, nullptr);
    return handle->get_constant_wave_controller();
}

Infineon::Avian::Constant_Wave_Controller* ifx_Radar_Device_s::get_constant_wave_controller()
{
    if (!m_cw_controller)
    {
        auto* avian_port = get_strata_avian_port();
        m_cw_controller = std::make_unique<Avian::Constant_Wave_Controller>(*avian_port);
    }

    return m_cw_controller.get();
}

//----------------------------------------------------------------------------

ifx_List_t* ifx_avian_get_hp_cutoff_list(const ifx_Avian_Device_t* handle)
{
    IFX_ERR_BRV_NULL(handle, nullptr);

    auto get_list = [&handle]() {
        return ifx_list_from_vector(handle->get_hp_cutoff_list(handle));
    };
    return rdk::RadarDeviceCommon::exec_func<ifx_List_t*>(get_list, nullptr);
}

//----------------------------------------------------------------------------

ifx_List_t* ifx_avian_get_aaf_cutoff_list(const ifx_Avian_Device_t* handle)
{
    IFX_ERR_BRV_NULL(handle, nullptr);

    auto get_list = [&handle]() {
        return ifx_list_from_vector(handle->get_aaf_cutoff_list(handle));
    };
    return rdk::RadarDeviceCommon::exec_func<ifx_List_t*>(get_list, nullptr);
}
