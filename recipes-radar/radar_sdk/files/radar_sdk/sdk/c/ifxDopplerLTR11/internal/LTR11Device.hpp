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


/**
 * @file LTR11Device.hpp
 *
 * \brief \copybrief gr_ltr11_device
 *
 * For details refer to \ref gr_ltr11_device
 */

#ifndef IFX_LTR11_DEVICE_HPP
#define IFX_LTR11_DEVICE_HPP

#include "ifxDopplerLTR11/internal/LTR11DeviceBase.hpp"
#include "ifxDopplerLTR11/internal/LTR11RegisterManipulator.hpp"
#include "ifxRadarDeviceCommon/internal/AcquisitionState.hpp"

// strata
#include <platform/BoardInstance.hpp>
#include <components/interfaces/IRegisters.hpp>
#include <universal/types/DataSettingsBgtRadar.h>

#include <memory>
#include <optional>

// forward declarations
class IRadarLtr11;
class IProtocolLtr11;


struct LTR11Device : public LTR11DeviceBase
{
public:
    LTR11Device(std::unique_ptr<BoardInstance> inBoard);
    ~LTR11Device() override;

    ifx_LTR11_Config_t getDefaultConfig() override;

    void setConfig(const ifx_LTR11_Config_t *config) override;

    void startAcquisition() override;
    void stopAcquisition() override;

    void dumpRegisters(const char* filename) override;

    ifx_Vector_C_t* getNextFrame(ifx_Vector_C_t* frame, ifx_LTR11_Metadata_t* metadata, uint16_t timeoutMs) override;

private:
    void softReset();

    ifx_Radar_Sensor_t getShieldType() const;

    uint32_t determineBufferSize() const;

    void setupConfig();

    void setupFrameData();

    void setupBridgeData();

    void setupMisoArbitration(uint16_t prt = 0);

    void startDataStreaming();
    void stopDataStreaming();

    uint16_t getNumberOfSamples() const;

    void readNextFrame(ifx_Vector_C_t* frameData, ifx_LTR11_Metadata_t* metadata, uint16_t timeoutMs);

    std::unique_ptr<BoardInstance> m_board;

    IRadarLtr11* m_ltr11;
    IBridgeData* m_bridgeData;
    IData* m_data;

    IProtocolLtr11* m_cmd;
    IRegisters<uint8_t, uint16_t>* m_reg;

    std::unique_ptr<LTR11RegisterManipulator> m_regMan;

    uint8_t m_dataIndex;

    uint32_t m_frameSize;

    std::optional<uint64_t> m_timestampPrev;

    std::atomic<bool> m_acquisitionStarted;

    ifx_Float_t m_frameCounter;
    ifx_Float_t m_averagePower;
};

#endif /* IFX_LTR11_DEVICE_HPP */
