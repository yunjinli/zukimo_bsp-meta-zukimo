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

#include "ifxBase/Math.h"
#include "ifxAvian/Metrics.h"
#include "internal/RadarDeviceBase.hpp"
#include "ifxRadarDeviceCommon/internal/RadarDeviceCommon.hpp"
#include "ifxAvian_DeviceTraits.hpp"

#include <cmath>

namespace
{
    // Get sampling delay
    inline ifx_Float_t get_adc_delay_s(const ifx_Avian_Device_t* avian)
    {
        return avian->get_chirp_timing().adc_delay_100ps * 100e-12f;
    }

    // Round up float to next number and return as uint32_t
    inline uint32_t ceil_u32(ifx_Float_t v)
    {
        return static_cast<uint32_t>(std::ceil(v));
    }
}

void ifx_avian_metrics_from_config(const ifx_Avian_Device_t* avian, const ifx_Avian_Config_t* config, ifx_Avian_Metrics_t* metrics)
{
    IFX_ERR_BRK_NULL(avian)
    IFX_ERR_BRK_NULL(config);
    IFX_ERR_BRK_NULL(metrics);

    // T_ADC
    const ifx_Float_t sampling_time_s = ifx_Float_t(config->num_samples_per_chirp) / config->sample_rate_Hz;
    const ifx_Float_t adc_delay_s = get_adc_delay_s(avian);

    const ifx_Float_t total_bandwidth_Hz = ifx_devconf_get_bandwidth(config);
    const ifx_Float_t sampling_bandwidth_Hz = total_bandwidth_Hz * sampling_time_s / (sampling_time_s + adc_delay_s);
    const ifx_Float_t center_frequency_Hz = ifx_devconf_get_center_frequency(config);
    const ifx_Float_t wavelength_m = IFX_LIGHT_SPEED_MPS / center_frequency_Hz;

    const ifx_Float_t range_resolution_m = IFX_LIGHT_SPEED_MPS / (2 * sampling_bandwidth_Hz);
    const ifx_Float_t max_range_m = config->num_samples_per_chirp * range_resolution_m / 2;

    const ifx_Float_t max_speed_m_s = wavelength_m / (4 * config->chirp_repetition_time_s);
    const ifx_Float_t speed_resolution_m_s = 2 * max_speed_m_s / config->num_chirps_per_frame;

    metrics->sample_rate_Hz = config->sample_rate_Hz;
    metrics->rx_mask = config->rx_mask;
    metrics->tx_mask = config->tx_mask;
    metrics->tx_power_level = config->tx_power_level;
    metrics->if_gain_dB = config->if_gain_dB;
    metrics->max_range_m = max_range_m;
    metrics->range_resolution_m = range_resolution_m;
    metrics->max_speed_m_s = max_speed_m_s;
    metrics->speed_resolution_m_s = speed_resolution_m_s;
    metrics->frame_repetition_time_s = config->frame_repetition_time_s;
    metrics->center_frequency_Hz = center_frequency_Hz;
}

void ifx_avian_metrics_to_config(const ifx_Avian_Device_t* avian, const ifx_Avian_Metrics_t* metrics, ifx_Avian_Config_t* config, bool round_to_power_of_2)
{
    IFX_ERR_BRK_NULL(avian);
    IFX_ERR_BRK_NULL(metrics);
    IFX_ERR_BRK_NULL(config);
    IFX_ERR_BRK_BIGGER_INCL(metrics->speed_resolution_m_s, metrics->max_speed_m_s);

    const auto* info = ifx_avian_get_sensor_information(avian);

    auto num_samples_per_chirp = ceil_u32(2 * metrics->max_range_m / metrics->range_resolution_m);
    auto num_chirps_per_frame = ceil_u32(2 * metrics->max_speed_m_s / metrics->speed_resolution_m_s);
    if (round_to_power_of_2)
    {
        num_samples_per_chirp = ifx_math_round_up_power_of_2_uint32(num_samples_per_chirp);
        num_chirps_per_frame = ifx_math_round_up_power_of_2_uint32(num_chirps_per_frame);
    }

    const ifx_Float_t sampling_time_s = ifx_Float_t(num_samples_per_chirp) / metrics->sample_rate_Hz;
    const ifx_Float_t adc_delay_s = get_adc_delay_s(avian);

    const ifx_Float_t sampling_bandwidth_Hz = IFX_LIGHT_SPEED_MPS / (2 * metrics->range_resolution_m);
    const ifx_Float_t total_bandwidth_Hz = sampling_bandwidth_Hz * (sampling_time_s + adc_delay_s)/ sampling_time_s;

    ifx_Float_t center_frequency_Hz;
    if (metrics->center_frequency_Hz != 0)
        center_frequency_Hz = metrics->center_frequency_Hz;
    else
        center_frequency_Hz = ifx_Float_t(info->min_rf_frequency_Hz + info->max_rf_frequency_Hz) / 2;

    const ifx_Float_t wavelength_m = IFX_LIGHT_SPEED_MPS / center_frequency_Hz;
    const ifx_Float_t chirp_repetition_time_s = wavelength_m / (4 * metrics->max_speed_m_s);

    const auto start_frequency_Hz = uint64_t(center_frequency_Hz - total_bandwidth_Hz / 2);
    const auto end_frequency_Hz = uint64_t(center_frequency_Hz + total_bandwidth_Hz / 2);
    IFX_ERR_BRK_COND(start_frequency_Hz < info->min_rf_frequency_Hz || end_frequency_Hz > info->max_rf_frequency_Hz, IFX_ERROR_RF_OUT_OF_RANGE);

	auto sensor_type = avian->get_sensor_type();
	auto avian_type = static_cast<Infineon::Avian::Device_Type>(sensor_type);
    const auto& device_traits = Avian::Device_Traits::get(avian_type);
    const auto& aaf_cutoff_settings = device_traits.aaf_cutoff_settings;
        
    config->sample_rate_Hz = metrics->sample_rate_Hz;
    config->rx_mask = metrics->rx_mask;
    config->tx_mask = metrics->tx_mask;
    config->tx_power_level = metrics->tx_power_level;
    config->if_gain_dB = metrics->if_gain_dB;
    config->start_frequency_Hz = start_frequency_Hz;
    config->end_frequency_Hz = end_frequency_Hz;
    config->num_samples_per_chirp = num_samples_per_chirp;
    config->num_chirps_per_frame = num_chirps_per_frame;
    config->chirp_repetition_time_s = chirp_repetition_time_s;
    config->frame_repetition_time_s = metrics->frame_repetition_time_s;
    config->hp_cutoff_Hz = 80000;
    config->aaf_cutoff_Hz = aaf_cutoff_settings.at(0) * 1000;
    config->mimo_mode = IFX_MIMO_OFF;
}

void ifx_avian_metrics_get_defaults(ifx_Avian_Device_t* avian, ifx_Avian_Metrics_t* metrics)
{
    IFX_ERR_BRK_NULL(avian);
    IFX_ERR_BRK_NULL(metrics);

    auto get_default_metrics = [&avian, &metrics]() {
        *metrics = avian->get_default_metrics();
    };

    rdk::RadarDeviceCommon::exec_func(get_default_metrics);
}

bool ifx_avian_metrics_get_limits(const ifx_Avian_Device_t* avian, const ifx_Avian_Metrics_t* metrics, ifx_Avian_Metrics_Limits_t* limits)
{
    // Amplification factor that is applied to the IF signal before sampling; Valid range: [18-60]dB
    constexpr uint32_t IFX_IF_GAIN_DB_LOWER_LIMIT = 18;
    constexpr uint32_t IFX_IF_GAIN_DB_UPPER_LIMIT = 60;

    double lower_bound;
    double upper_bound;

    IFX_ERR_BRV_NULL(avian, false);
    IFX_ERR_BRV_NULL(metrics, false);
    IFX_ERR_BRV_NULL(limits, false);

    /* initialize limits with zeros. In case we forget to set a field, min and
     * max will be 0 which makes potential bugs easier to find. */
    *limits = {};

    const ifx_Avian_Calc_t* calc = avian->get_device_calc();
    const ifx_Radar_Sensor_Info_t* sensor_info = avian->get_sensor_info();

    /* rx mask is within [1, 2**num_rx_antennas-1] */
    limits->rx_mask.min = 1;
    limits->rx_mask.max = (1 << sensor_info->num_rx_antennas) - 1;

    /* tx mask is within [1, 2**num_tx_antennas-1] */
    limits->tx_mask.min = 0;
    limits->tx_mask.max = (1 << sensor_info->num_tx_antennas) - 1;

    /* tx_power_level is within [0, sensor_info->max_tx_power] */
    limits->tx_power_level.min = 0;
    limits->tx_power_level.max = sensor_info->max_tx_power;

    limits->if_gain_dB.min = IFX_IF_GAIN_DB_LOWER_LIMIT;
    limits->if_gain_dB.max = IFX_IF_GAIN_DB_UPPER_LIMIT;

    /* get bounds for ADC sample rate. If sample rate in metrics structure is
     * outside the allowed range, set it to the closest allowed value.
     */
    uint32_t sample_rate = metrics->sample_rate_Hz;
    {
        uint32_t sample_rate_min;
        uint32_t sample_rate_max;
        ifx_avian_calc_sample_rate_bounds(calc, &sample_rate_min, &sample_rate_max);
        limits->sample_rate_Hz.min = sample_rate_min;
        limits->sample_rate_Hz.max = sample_rate_max;

        if (sample_rate < sample_rate_min)
            sample_rate = sample_rate_min;
        else if (sample_rate > sample_rate_max)
            sample_rate = sample_rate_max;
    }

    /* To avoid floating point problems due to rounding, we output a bit
     * more conservative limits and make the lower bounds a bit higher
     * and the upper bounds a bit lower than actually allowed. */
    constexpr ifx_Float_t epsp1 = 1.0001f;

    // lambda clamp
    auto clamp = [](ifx_Float_t value, ifx_Float_t min, ifx_Float_t max) -> ifx_Float_t
    {
        if (value < min)
            return min;
        if (value > max)
            return max;
        return value;
    };

    /* Compute bounds for range_resolution, max_range, max_speed, speed_resolution.
     *
     * 1. range_resolution: The bounds only on the maximum bandwidth of the
     *    device.
     * 2. max_range: Depends on range_resolution.
     * 3. max_speed: Depends on max_range and range_resolution.
     * 4. speed_resolution: Depends on max_speed (and implicitly on
     *    range_resolution and max_range).
     * 5. frame_repetition_time: Depends on range_resolution, max_range,
     *    max_speed, speed_resolution.
     */

    ifx_avian_calc_range_resolution_bounds(calc, &lower_bound, &upper_bound);
    limits->range_resolution_m.min = static_cast<ifx_Float_t>(lower_bound) * epsp1;
    limits->range_resolution_m.max = static_cast<ifx_Float_t>(upper_bound) / epsp1;
    const double range_resolution = clamp(metrics->range_resolution_m, limits->range_resolution_m.min, limits->range_resolution_m.max);

    ifx_avian_calc_max_range_bounds(calc, range_resolution, &lower_bound, &upper_bound);
    limits->max_range_m.min = static_cast<ifx_Float_t>(lower_bound) * epsp1;
    limits->max_range_m.max = static_cast<ifx_Float_t>(upper_bound) / epsp1;
    const double max_range = clamp(metrics->max_range_m, limits->max_range_m.min, limits->max_range_m.max);

    const uint32_t num_samples_per_chirp = ifx_avian_calc_num_samples_per_chirp(calc, max_range, range_resolution);
    const double sampling_time = ifx_avian_calc_sampling_time(calc, num_samples_per_chirp, sample_rate);

    const double samplingBandwidth = ifx_avian_calc_sampling_bandwidth(calc, range_resolution);

    const double chirp_time = ifx_avian_calc_chirp_time(calc, sampling_time);
    const double totalBandwidth = ifx_avian_calc_total_bandwidth(calc, sampling_time, chirp_time, samplingBandwidth);
    const double center_frequency = ifx_avian_calc_center_frequency(calc, totalBandwidth);
    const double wavelength = ifx_avian_calc_wavelength(calc, center_frequency);

    ifx_avian_calc_max_speed_bounds(calc, wavelength, sampling_time, &lower_bound, &upper_bound);
    limits->max_speed_m_s.min = static_cast<ifx_Float_t>(lower_bound) * epsp1;
    limits->max_speed_m_s.max = static_cast<ifx_Float_t>(upper_bound) / epsp1;
    const double max_speed = clamp(metrics->max_speed_m_s, limits->max_speed_m_s.min, limits->max_speed_m_s.max);

    ifx_avian_calc_speed_resolution_bounds(calc, max_speed, &lower_bound, &upper_bound);
    limits->speed_resolution_m_s.min = static_cast<ifx_Float_t>(lower_bound) * epsp1;
    limits->speed_resolution_m_s.max = static_cast<ifx_Float_t>(upper_bound) / epsp1;
    const double speed_resolution = clamp(metrics->speed_resolution_m_s, limits->speed_resolution_m_s.min, limits->speed_resolution_m_s.max);

    const double chirp_repetition_time = ifx_avian_calc_chirp_repetition_time(calc, max_speed, wavelength);
    uint32_t num_chirps_per_frame = ifx_avian_calc_num_chirps_per_frame(calc, max_speed, speed_resolution);
    ifx_avian_calc_frame_repetition_time_bounds(calc, sampling_time, chirp_repetition_time, num_chirps_per_frame, &lower_bound, &upper_bound);
    limits->frame_repetition_time_s.min = static_cast<ifx_Float_t>(lower_bound) * epsp1;
    limits->frame_repetition_time_s.max = static_cast<ifx_Float_t>(upper_bound) / epsp1;

    /* check if input metrics are valid (within bounds of limits) */
    if (metrics->sample_rate_Hz < limits->sample_rate_Hz.min || metrics->sample_rate_Hz > limits->sample_rate_Hz.max)
        return false;
    if (metrics->rx_mask < limits->rx_mask.min || metrics->rx_mask > limits->rx_mask.max)
        return false;
    if (metrics->tx_mask < limits->tx_mask.min || metrics->tx_mask > limits->tx_mask.max)
        return false;
    if (metrics->tx_power_level < limits->tx_power_level.min || metrics->tx_power_level > limits->tx_power_level.max)
        return false;
    if (metrics->if_gain_dB < limits->if_gain_dB.min || metrics->if_gain_dB > limits->if_gain_dB.max)
        return false;

    if (metrics->range_resolution_m < limits->range_resolution_m.min || metrics->range_resolution_m > limits->range_resolution_m.max)
        return false;
    if (metrics->max_range_m < limits->max_range_m.min || metrics->max_range_m > limits->max_range_m.max)
        return false;
    if (metrics->max_speed_m_s < limits->max_speed_m_s.min || metrics->max_speed_m_s > limits->max_speed_m_s.max)
        return false;
    if (metrics->speed_resolution_m_s < limits->speed_resolution_m_s.min || metrics->speed_resolution_m_s > limits->speed_resolution_m_s.max)
        return false;
    if (metrics->frame_repetition_time_s < limits->frame_repetition_time_s.min || metrics->frame_repetition_time_s > limits->frame_repetition_time_s.max)
        return false;

    return true;
}