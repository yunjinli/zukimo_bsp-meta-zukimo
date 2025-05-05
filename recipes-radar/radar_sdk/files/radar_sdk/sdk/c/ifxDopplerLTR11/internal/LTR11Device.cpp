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

#include "LTR11Device.hpp"
#include "LTR11DeviceBase.hpp"
#include "LTR11DeviceConstants.hpp"


#include <common/exception/EException.hpp>
#include <components/interfaces/IRadarLtr11.hpp>
#include <platform/interfaces/IBridgeControl.hpp>
#include <platform/interfaces/IBridgeData.hpp>

#include "ifxBase/Exception.hpp"
#include "ifxBase/Mem.h"
#include "ifxBase/Vector.h"
#include "ifxRadarDeviceCommon/internal/RadarDeviceCommon.hpp"

#include <array>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <numeric>
#include <unordered_map>
#include <vector>


namespace
{
    constexpr uint16_t DEFAULT_QUEUE_SIZE = 4096;

    constexpr uint16_t BGT60LTR11_ADC_RSL_IFI_REG40_REG_ADDR   = 0x28;
    constexpr uint16_t BGT60LTR11_STS_CHIP_VER_REG56_REG_ADDR  = 0x38;
    constexpr uint16_t DATA_LTR11_DETECTOR_OUTPUT_VIRTUAL_ADDR = 0xFF;

    constexpr uint8_t IFX_LTR11_DETECTOR_OUTPUT_MOTION_MASK    = 1 << 0;
    constexpr uint8_t IFX_LTR11_DETECTOR_OUTPUT_DIRECTION_MASK = 1 << 1;

    constexpr std::array<uint16_t, 3> minVersion = {2, 5, 4};

    using ReadoutDataConfiguration = std::vector<ReadoutEntry_t>;

    const IDataProperties_t properties = {};

    const ReadoutDataConfiguration frameReadoutConfiguration {
        {BGT60LTR11_ADC_RSL_IFI_REG40_REG_ADDR, 2},    // ADC Ifi Samples
        {BGT60LTR11_STS_CHIP_VER_REG56_REG_ADDR, 1},   // Amplitude
        {DATA_LTR11_DETECTOR_OUTPUT_VIRTUAL_ADDR, 1},  // Detector Output
    };

    uint32_t getFrameSize()
    {
        uint32_t frameSize = 0;

        for (const auto &readoutConfig : frameReadoutConfiguration)
        {
            frameSize += (readoutConfig.count);
        }

        return (frameSize * sizeof(uint16_t));
    }

    float normalize(uint16_t fvalue)
    {
        /* For LTR11, internal ADC, which is physically an 8bit ADC,
     * is used. The result is 10 bit wide, but only bit9-bit2 are
     * significant, hence bit1-bit0 and bit15-bit10 are discarded.
    */
        auto value = (fvalue & 0x3FC) >> 2;
        return (1.0f * value) / 0xFF;
    }

}  // end of anonymous namespace


LTR11Device::LTR11Device(std::unique_ptr<BoardInstance> inBoard) :
    m_board(std::move(inBoard)),
    m_ltr11(nullptr),
    m_bridgeData(nullptr),
    m_data(nullptr),
    m_cmd(nullptr),
    m_reg(nullptr),
    m_dataIndex(0),
    m_frameSize(::getFrameSize()),
    m_acquisitionStarted(false),
    m_frameCounter(0),
    m_averagePower(0)
{
    if (!m_board)
        throw rdk::exception::no_device();

    // Check that we are really connected to a board with an LTR11 sensor
    if (m_board->getComponentImplementation<IRadarLtr11>(m_dataIndex) != IRadarLtr11::getImplementation())
        throw rdk::exception::not_supported();

    if (!m_board->getIBridge() || !m_board->getIBridge()->isConnected())
    {
        throw rdk::exception::communication_error();
    }

    m_ltr11 = m_board->getComponent<IRadarLtr11>(m_dataIndex);

    if (m_ltr11 == nullptr)
    {
        throw rdk::exception::no_device();
    }

    m_bridgeData = m_board->getIBridge()->getIBridgeData();
    m_data       = m_board->getIBridge()->getIBridgeControl()->getIData();

    // get the ltr1122 resources
    m_cmd = m_ltr11->getIProtocolLtr11();
    m_reg = m_ltr11->getIRegisters();

    // probe the shield type
    if (getShieldType() != IFX_BGT60LTR11AIP)
    {
        throw ::rdk::exception::not_supported();
    }

    m_regMan = std::make_unique<LTR11RegisterManipulator>(m_reg);

    m_bandJapan = m_regMan->isJapanBand();

    /* check if firmware is valid */
    {
        rdk::RadarDeviceCommon::get_firmware_info(m_board.get(), &m_firmware_info);
        if (!rdk::RadarDeviceCommon::is_firmware_version_valid(minVersion, m_firmware_info))
            throw rdk::exception::firmware_version_not_supported();
    }
}

LTR11Device::~LTR11Device()
{
    try
    {
        stopAcquisition();
    }
    catch (...)
    {
        // It might happen that stopAcquisition throws an exception in case
        // the device is no longer present.
        //
        // As a destructor must not throw exceptions, we ignore the exception here.
        //
        // Anyhow, if the device is longer present, it is also not necessary to
        // stop the acquisition.
    }
}


ifx_LTR11_Config_t LTR11Device::getDefaultConfig()
{
    ifx_LTR11_Config_t config = LTR11DeviceBase::getDefaultConfig();

    return config;
}

void LTR11Device::setConfig(const ifx_LTR11_Config_t *config)
{
    if (m_acquisitionStarted)
    {
        throw rdk::exception::not_supported();
    }

    LTR11DeviceBase::setConfig(config);

    if (!m_config)
    {
        throw rdk::exception::not_supported();
    }

    softReset();

    setupMisoArbitration(0);

    setupConfig();

    if (config->disable_internal_detector)
    {
        m_regMan->disableInternalDetector();
    }

    setupMisoArbitration(LTR11DeviceBase::prtIndexToUs(m_config->prt));

    setupFrameData();

    setupBridgeData();
}

void LTR11Device::startAcquisition()
{
    if (m_acquisitionStarted)
    {
        return;
    }
    if (!m_config)
    {
        throw rdk::exception::not_configured();
    }
    startDataStreaming();

    // Re-initialize member variables needed to compute the metadata
    m_timestampPrev.reset();
    m_frameCounter = 0;
    m_averagePower = 0;

    m_acquisitionStarted = true;
}

void LTR11Device::stopAcquisition()
{

    if(!m_acquisitionStarted){
        return;
    }

    stopDataStreaming();
    m_acquisitionStarted = false;
}

ifx_Vector_C_t *LTR11Device::getNextFrame(ifx_Vector_C_t *frameData, ifx_LTR11_Metadata_t *metadata, uint16_t timeoutMs)
{
    if (!m_acquisitionStarted)
    {
        throw ::rdk::exception::error();
    }

    bool frameDataMemoryAllocated; // Flag indicating when true: frame memory allocated in getNextFrame method
    if (!frameData)
    {
        //allocate memory for frame
        frameData = ifx_vec_create_c(getNumberOfSamples());
        if (!frameData)
            throw rdk::exception::memory_allocation_failed();
        frameDataMemoryAllocated = true;
    }
    else
    {   
        if (IFX_VEC_LEN(frameData) != getNumberOfSamples())
            throw rdk::exception::dimension_mismatch();
        frameDataMemoryAllocated = false;
    }

    try
    {
        readNextFrame(frameData, metadata, timeoutMs);
    }
    catch (const rdk::exception::exception &e)
    {
        stopAcquisition();
        if (frameDataMemoryAllocated)
            ifx_vec_destroy_c(frameData);
        throw e;
    }

    return frameData;
}

void LTR11Device::softReset()
{
    m_ltr11->getIPinsLtr11()->reset();
}

ifx_Radar_Sensor_t LTR11Device::getShieldType() const
{
    // TODO(benhmidaines): determine the shield type.
    return IFX_BGT60LTR11AIP;
}

uint32_t LTR11Device::determineBufferSize() const
{
    if ((getNumberOfSamples() == 0) || (getNumberOfSamples() > IFX_LTR11_MAX_ALLOWED_NUM_SAMPLES))
    {
        throw rdk::exception::not_supported();
    }

    return (m_frameSize * getNumberOfSamples());
}

void LTR11Device::setupConfig()
{
    // at first apply the configuration
    m_regMan->addSetDefaultRegistersList();
    m_regMan->addSetPulseConfig(m_config->prt, m_config->pulse_width, m_config->tx_power_level);
    m_regMan->addSetHoldTime(m_config->hold_time);
    m_regMan->addSetDetectorThreshold_AdaptivPRT(m_config->internal_detector_threshold, m_config->aprt_factor);
    m_regMan->addSetRX_IF_Gain(m_config->rx_if_gain);
    m_regMan->addSetRFCenterFrequency(m_config->rf_frequency_Hz);
    m_regMan->addSetPulseMode(m_config->mode);
    m_regMan->flushEnqRegisters();
}

void LTR11Device::setupFrameData()
{
    DataSettingsBgtRadar_t settings;
    settings.initialize(frameReadoutConfiguration, getNumberOfSamples());
    m_data->configure(m_dataIndex, &properties, &settings);
}

void LTR11Device::setupBridgeData()
{
    m_bridgeData->setFrameBufferSize(m_frameSize * getNumberOfSamples());
    m_bridgeData->setFrameQueueSize(DEFAULT_QUEUE_SIZE);
}

void LTR11Device::setupMisoArbitration(uint16_t prt /* = 0*/)
{
    m_cmd->setMisoArbitration(prt);
}

void LTR11Device::startDataStreaming()
{
    m_bridgeData->startStreaming();
    m_data->start(m_dataIndex);
}

void LTR11Device::stopDataStreaming()
{
    m_data->stop(m_dataIndex);
    m_bridgeData->stopStreaming();
}

uint16_t LTR11Device::getNumberOfSamples() const
{
    return m_config->num_of_samples;
}

void checkStatusCodeOrFrameBufferSize(uint32_t statusCode, uint32_t frameBufferSize, uint32_t expectedFrameBufferSize)
{
    if (statusCode != 0)
    {
        if (statusCode == DataError_FramePoolDepleted)
        {
            throw rdk::exception::fifo_overflow();
        }
        else if (statusCode == DataError_FrameDropped)
        {
            throw rdk::exception::frame_acquisition_failed();
        }
        else
        {
            throw rdk::exception::internal();
        }
        return;
    }

    if (frameBufferSize != expectedFrameBufferSize)
    {
        throw rdk::exception::frame_acquisition_failed();
    }
}

void LTR11Device::readNextFrame(ifx_Vector_C_t *frameData, ifx_LTR11_Metadata_t *metadata, uint16_t timeoutMs)
{
    const auto frameBufferSize = determineBufferSize();

    auto *deviceFrame = m_board->getFrame(timeoutMs);

    if (deviceFrame == nullptr)
    {
        throw rdk::exception::timeout();
    }

    uint32_t statusCode = deviceFrame->getStatusCode();

    try
    {
        checkStatusCodeOrFrameBufferSize(statusCode, deviceFrame->getDataSize(), frameBufferSize);
    }
    catch (const rdk::exception::exception &e)
    {
        deviceFrame->release();
        stopAcquisition();
        throw e;
    }


    auto frameTimestamp = deviceFrame->getTimestamp();
    if (m_timestampPrev)
    {
        metadata->active = (frameTimestamp - *m_timestampPrev) < m_timestampThreshold;
    }
    else
    {
        /* There is no previous timestamp for the first frame, and the chip should be by
         * default in active mode. Hence, for the first frame, active should be initialized
         * to true (similarly the average power should be the active mode power). */
        metadata->active = true;
        m_averagePower   = m_activePower;
    }
    m_timestampPrev = frameTimestamp;

    ifx_Float_t currentPower;
    if (metadata->active)
    {
        currentPower = m_activePower;
    }
    else
    {
        currentPower = m_lowPower;
    }
    m_averagePower += (currentPower - m_averagePower) / ++m_frameCounter;

    const auto frameStepping       = (m_frameSize / sizeof(uint16_t));
    const auto detectorOutputIndex = 3;  // detector output index in data readout

    const auto *dataAsUint = reinterpret_cast<const uint16_t *>(deviceFrame->getData());

    const auto numberOfSamples = getNumberOfSamples();
    for (size_t i = 0; i < numberOfSamples; ++i)
    {

        const auto ifiIdx = (i * frameStepping);
        // ifqIdx  = ifiIdx + 1

        const ifx_Float_t I = ::normalize(dataAsUint[ifiIdx]);
        const ifx_Float_t Q = ::normalize(dataAsUint[ifiIdx + 1]);

        IFX_VEC_AT(frameData, i) = ifx_Complex_t {I, Q};
    }

    metadata->motion    = (dataAsUint[(numberOfSamples - 1) * frameStepping + detectorOutputIndex] & IFX_LTR11_DETECTOR_OUTPUT_MOTION_MASK) == IFX_LTR11_DETECTOR_OUTPUT_MOTION_MASK;
    metadata->direction = (dataAsUint[(numberOfSamples - 1) * frameStepping + detectorOutputIndex] & IFX_LTR11_DETECTOR_OUTPUT_DIRECTION_MASK) == IFX_LTR11_DETECTOR_OUTPUT_DIRECTION_MASK;
    metadata->avg_power = m_averagePower;

    deviceFrame->release();
}

void LTR11Device::dumpRegisters(const char *filename)
{
    m_regMan->dumpRegisters(filename);
}