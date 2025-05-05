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

#include "LTR11DeviceBase.hpp"
#include "LTR11DeviceConstants.hpp"
#include "ifxBase/Exception.hpp"

#include <cassert>
#include <cmath>

namespace
{
    const ifx_Float_t avgIdleTimeCurrentmA  = 0.8f;                // Average Idle Time Current in mA
    const uint8_t activePulseTimeUs         = 15;                  // Active time in us
    const uint8_t avgActiveTimeCurrentmA[4] = {100, 142, 83, 91};  // Average Current for entire pulse duration in mA
    const ifx_Float_t voltageV              = 1.5f;                // The outputs from MMIC (Tdet and Pdet pins) are at the voltage level of 1.5 V
}

ifx_LTR11_Config_t LTR11DeviceBase::m_defaultConfig = {
    IFX_LTR11_SPI_PULSE_MODE,                  /* .mode */
    0,                                         /* .rf_frequency_Hz */
    LTR11_NUM_OF_SAMPLES_DEFAULT,              /* .num_of_samples */
    LTR11_INTERNAL_DETECTOR_THRESHOLD_DEFAULT, /* .internal_detector_threshold */
    IFX_LTR11_PRT_500us,                       /* .prt */
    IFX_LTR11_PULSE_WIDTH_5us,                 /* .pulse_width */
    IFX_LTR11_TX_POWER_LEVEL_4_5dBm,           /* .tx_power_level */
    IFX_LTR11_RX_IF_GAIN_50dB,                 /* .rx_if_gain */
    IFX_LTR11_APRT_FACTOR_1,                   /* .aprt_factor */
    IFX_LTR11_HOLD_TIME_1s,                    /* .hold_time */
    false,                                     /* .disable_internal_detector */
};

template<typename T>
static bool checkInRange(T min, T x, T max){
    return min<=x && x<=max;
}

template<typename T>
static bool checkStep(T x, T step)
{
    return (x % step) == 0;
}

ifx_LTR11_Config_t LTR11DeviceBase::getConfig(){
    if(!m_config){
        getDefaultConfig();
    }
    return *m_config;
}

void LTR11DeviceBase::setConfig(const ifx_LTR11_Config_t *config){
    if (checkConfig(config))
    {
        m_config = std::optional<ifx_LTR11_Config_t>(*config);

        const auto aprtFactor     = aprtFactorValue(config->aprt_factor);
        const auto num_of_samples = config->num_of_samples;
        const auto prt            = prtIndexToUs(config->prt);
        const auto pulseWidth     = config->pulse_width;

        m_activePower   = getPowerConsumption(pulseWidth, prt);
        m_lowPower      = getPowerConsumption(pulseWidth, aprtFactor * prt);
        /* Tolerance of 10us, to cover the timestamp delta measurement deviation (dependant on the potential
         *  prt measurement deviations) when the chip is in active mode.*/
        const decltype(m_timestampThreshold) tolerance = 10;
        m_timestampThreshold = prt * num_of_samples + tolerance;
    }
    else
    {
        throw rdk::exception::argument_invalid();
    }
}

void LTR11DeviceBase::getLimits(ifx_LTR11_Config_Limits_t *limits)
{
    if (!limits)
    {
        throw rdk::exception::argument_invalid();
    }

    uint64_t freqMin;
    uint64_t freqMax;

    if (m_bandJapan)
    {
        freqMin = LTR11_FREQUENCY_MIN_JAPAN;
        freqMax = LTR11_FREQUENCY_MAX_JAPAN;
    }
    else
    {
        freqMin = LTR11_FREQUENCY_MIN;
        freqMax = LTR11_FREQUENCY_MAX;
    }

    *limits = {
        {freqMin, freqMax, LTR11_FREQUENCY_STEP},                    /* .rf_frequency_Hz */
        {LTR11_NUM_OF_SAMPLES_MIN, LTR11_NUM_OF_SAMPLES_MAX},        /* .num_of_samples */
        {LTR11_INTERNAL_DETECTOR_THRESHOLD_MIN, LTR11_INTERNAL_DETECTOR_THRESHOLD_MAX} /* .internal_detector_threshold */
    };
}

bool LTR11DeviceBase::checkConfig(const ifx_LTR11_Config_t *config)
{
    ifx_LTR11_Config_Limits_t limits;
    getLimits(&limits);

    return checkInRange(limits.rf_frequency_Hz.min, config->rf_frequency_Hz, limits.rf_frequency_Hz.max) &&
           checkStep(config->rf_frequency_Hz, limits.rf_frequency_Hz.step) &&
           checkInRange(limits.internal_detector_threshold.min, config->internal_detector_threshold, limits.internal_detector_threshold.max) &&
           checkInRange(limits.num_of_samples.min, config->num_of_samples, limits.num_of_samples.max);
}

ifx_LTR11_Config_t LTR11DeviceBase::getDefaultConfig()
{
    if (m_bandJapan)
    {
        m_defaultConfig.rf_frequency_Hz = LTR11_FREQUENCY_DEFAULT_JAPAN;
    }
    else
    {
        m_defaultConfig.rf_frequency_Hz = LTR11_FREQUENCY_DEFAULT;
    }

    return m_defaultConfig;
}

const ifx_Radar_Sensor_Info_t* LTR11DeviceBase::get_sensor_info()
{
    if (m_bandJapan)
    {
        m_sensor_info.min_rf_frequency_Hz = LTR11_FREQUENCY_MIN_JAPAN;
        m_sensor_info.max_rf_frequency_Hz = LTR11_FREQUENCY_MAX_JAPAN;
    }
    else
    {
        m_sensor_info.min_rf_frequency_Hz = LTR11_FREQUENCY_MIN;
        m_sensor_info.max_rf_frequency_Hz = LTR11_FREQUENCY_MAX;
    }
    m_sensor_info.description      = "BGT60LTR11AIP";
    m_sensor_info.num_tx_antennas  = 1;
    m_sensor_info.num_rx_antennas  = 1;
    m_sensor_info.max_tx_power     = 7;
    m_sensor_info.num_temp_sensors = 0;
    m_sensor_info.interleaved_rx   = 0;
    m_sensor_info.device_id        = 0;

    return &m_sensor_info;
}

const ifx_Firmware_Info_t* LTR11DeviceBase::get_firmware_info() const
{
    return &m_firmware_info;
}

ifx_Float_t LTR11DeviceBase::getActiveModePower(const ifx_LTR11_Config_t *config)
{
    const auto pulseWidth   = config->pulse_width;
    const auto prt          = prtIndexToUs(config->prt);

    return getPowerConsumption(pulseWidth, prt);
}

ifx_Float_t LTR11DeviceBase::getPowerConsumption(ifx_LTR11_PulseWidth_t pulseWidth, uint16_t prtValue)
{
    return voltageV * (avgActiveTimeCurrentmA[pulseWidth] * 1e-3f * activePulseTimeUs + avgIdleTimeCurrentmA * 1e-3f * (prtValue - activePulseTimeUs)) / prtValue;
}

uint16_t LTR11DeviceBase::prtIndexToUs(ifx_LTR11_PRT_t prtIndex)
{
    switch (prtIndex)
    {
        case IFX_LTR11_PRT_250us:
            return 250;
        case IFX_LTR11_PRT_500us:
            return 500;
        case IFX_LTR11_PRT_1000us:
            return 1000;
        case IFX_LTR11_PRT_2000us:
            return 2000;
        default:
            throw rdk::exception::argument_invalid();
    }
}

uint8_t LTR11DeviceBase::aprtFactorValue(ifx_LTR11_APRT_Factor_t aprtFactorIndex)
{
    switch (aprtFactorIndex)
    {
        case IFX_LTR11_APRT_FACTOR_16:
            return 16;
        case IFX_LTR11_APRT_FACTOR_8:
            return 8;
        case IFX_LTR11_APRT_FACTOR_4:
            return 4;
        case IFX_LTR11_APRT_FACTOR_2:
            return 2;
        case IFX_LTR11_APRT_FACTOR_1:
            return 1;
        default:
            throw rdk::exception::argument_invalid();
    }
}

uint32_t LTR11DeviceBase::getSamplingFrequency(ifx_LTR11_PRT_t prtIndex)
{
    return 1000000u / prtIndexToUs(prtIndex);
}
