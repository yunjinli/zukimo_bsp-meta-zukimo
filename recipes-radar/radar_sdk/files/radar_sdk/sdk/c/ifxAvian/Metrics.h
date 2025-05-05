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
 * @file Metrics.h
 *
 * \brief \copybrief gr_avian_metrics
 *
 * For details refer to \ref gr_avian_metrics
 */

#ifndef IFX_AVIAN_METRICS_H
#define IFX_AVIAN_METRICS_H

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#include "ifxBase/Types.h"
#include "DeviceControl.h"

/** @addtogroup gr_cat_Avian
 * @{
 */

/** @defgroup gr_avian_metrics Metrics
 * @brief API for Metrics
 *
 * The metrics provide a very easy way to configure the radar sensor in terms
 * of maximum range, maximum speed, range resolution, and speed
 * resolution that one wants to achieve. The metrics parameters in \ref ifx_Avian_Metrics_t can be converted
 * into a Avian device configuration \ref ifx_Avian_Config_t and vice versa.
 *
 * @{
 */

typedef struct
{
    struct { uint32_t min; uint32_t max; } sample_rate_Hz;
    struct { uint32_t min; uint32_t max; } rx_mask;
    struct { uint32_t min; uint32_t max; } tx_mask;
    struct { uint32_t min; uint32_t max; } tx_power_level;
    struct { uint32_t min; uint32_t max; } if_gain_dB;
    struct { ifx_Float_t min; ifx_Float_t max; } max_range_m;
    struct { ifx_Float_t min; ifx_Float_t max; } range_resolution_m;
    struct { ifx_Float_t min; ifx_Float_t max; } speed_resolution_m_s;
    struct { ifx_Float_t min; ifx_Float_t max; } max_speed_m_s;
    struct { ifx_Float_t min; ifx_Float_t max; } frame_repetition_time_s;
} ifx_Avian_Metrics_Limits_t;

/**
 * @brief Defines the structure for the metrics.
 *
 * For details on the conversion formulae refer to \ref sct_radarsdk_introduction_metrics
 */
typedef struct
{
    uint32_t sample_rate_Hz; /**< Sampling rate of the ADC used to acquire the samples
                                  during a chirp. The duration of a single chirp depends
                                  on the number of samples and the sampling rate. */

    uint32_t rx_mask;        /**< Bitmask where each bit represents one RX antenna of
                                  the radar device. If a bit is set the according RX
                                  antenna is enabled during the chirps and the signal
                                  received through that antenna is captured. The least
                                  significant bit corresponds to antenna 1. */
    uint32_t tx_mask;        /**< Bitmask where each bit represents one TX antenna. */
    uint32_t tx_power_level; /**< This value controls the power of the transmitted RX
                                  signal. This is an abstract value between 0 and 31
                                  without any physical meaning. */
    uint32_t if_gain_dB;     /**< Amplification factor that is applied to the IF signal
                                  coming from the RF mixer before it is fed
                                  into the ADC (allowed values in the range
                                  [0,60]). HP gain (allowed values: 0dB, 30dB)
                                  and VGA gain (allowed values: 0dB, 5dB, 10dB,
                                  ..., 30dB) are computed in a way that the sum
                                  of HP gain and VGA gain is as close to
                                  if_gain_dB as possible. */

    ifx_Float_t range_resolution_m;   /**< The range resolution is the distance between two consecutive
                                           bins of the range transform. Note that even though zero
                                           padding before the range transform seems to increase this
                                           resolution, the true resolution does not change but depends
                                           only from the acquisition parameters. Zero padding is just
                                           interpolation! */
    ifx_Float_t max_range_m;          /**< The bins of the range transform represent the range
                                           between 0m and this value. (If the time domain input data it
                                           is the range-max_range_m ... max_range_m.) */
    ifx_Float_t max_speed_m_s;        /**< The bins of the Doppler transform represent the speed values
                                           between -max_speed_m_s and max_speed_m_s. */
    ifx_Float_t speed_resolution_m_s; /**< The speed resolution is the distance between two consecutive
                                           bins of the Doppler transform. Note that even though zero
                                           padding before the speed transform seems to increase this
                                           resolution, the true resolution does not change but depends
                                           only from the acquisition parameters. Zero padding is just
                                           interpolation! */

    ifx_Float_t frame_repetition_time_s; /**< The desired frame repetition time in seconds (also known
                                              as frame time or frame period). The frame repetition time
                                              is the inverse of the frame rate */

    ifx_Float_t center_frequency_Hz; /**< Center frequency of the FMCW chirp. If the value is set to 0
                                          the center of the RF band (middle of minimum and maximum
                                          supported RF frequency) will be used as center frequency. */
} ifx_Avian_Metrics_t;


/**
 * @brief Determines metrics parameters from Avian device configuration
 *
 * The function converts the radar configuration in config to a metrics
 * structure.
 *
 * @param [in]   avian       Avian device handle
 * @param [in]   config      Avian device configuration structure
 * @param [out]  metrics     Metrics structure
 */
IFX_DLL_PUBLIC
void ifx_avian_metrics_from_config(const ifx_Avian_Device_t* avian, const ifx_Avian_Config_t* config, ifx_Avian_Metrics_t* metrics);

/**
 * @brief Translates metrics parameters to Avian device configuration
 *
 * The function converts the metrics parameters in metrics to an Avian device
 * configuration. If round_to_power_of_2 is true, the computed values for
 * num_samples_per_chirp and num_chirps_per_frame will be rounded to the next
 * power of 2.
 *
 * Changes:
 * * Radar SDK 3.4: If metrics.center_frequency_Hz is set to 0 the center of the RF
 *   band (the middle of the minimum and maximum supported RF frequency) is used as
 *   center frequency. Prior to Radar SDK 3.4 61.25GHz the middle of the ISM band
 *   was used if for bandwidths of less than 500MHz and the center of the RF band
 *   otherwise.
 *
 * @param [in]  avian                   Avian device handle
 * @param [in]  metrics                 Metrics structure
 * @param [out] config                  Avian device configuration structure
 * @param [in]  round_to_power_of_2     If true num_samples_per_chirp and
                                        num_chirps_per_frame are rounded to
                                        the next power of 2.
 */
IFX_DLL_PUBLIC
void ifx_avian_metrics_to_config(const ifx_Avian_Device_t* avian, const ifx_Avian_Metrics_t* metrics, ifx_Avian_Config_t* config, bool round_to_power_of_2);

/**
* @brief Get default metrics
*
* Returns a valid metrics structure for the current Avian sensor.
*
* If the function succeeds, the metrics is copied to the *metrics* output parameter, otherwise an
* error is set.
*
* \note The returned structure might differ for different releases.
*
* @param [in]     avian      Avian device handle
* @param [out]    metrics    The default metrics of the device.
*
*/
IFX_DLL_PUBLIC
void ifx_avian_metrics_get_defaults(ifx_Avian_Device_t* avian, ifx_Avian_Metrics_t* metrics);

/**
 * @brief Get the limits for the Metrics parameters
 *
 * Retrieve the limits (lower and upper bounds) for the parameters in
 * ifx_Avian_Metrics_t.
 *
 * The output limits depend on the input metrics. Different inputs for
 * metrics will lead to different bounds in limits.
 *
 * The limits depend on:
 *   - sample_rate_Hz depends on the connected the device device.
 *   - rx_mask depends on the connected radar device.
 *   - tx_mask depends on the connected radar device.
 *   - tx_power_level on the connected radar device.
 *   - if_gain_dB on the connected radar device.
 *   - range_resolution_m depends on the maximum bandwidth of the connected radar device.
 *   - max_range_m depends on range_resolution_m.
 *   - max_speed_m_s depends on max_range_m and range_resolution_m.
 *   - speed_resolution_m_s depends on max_speed_m_s (and implicitly on range_resolution_m and max_range_m).
 *   - frame_repetition_time_s depends on range_resolution_m, max_range_m, max_speed_m_s, speed_resolution_m_s.
 *
 * If all values in metrics are within the bounds computed in limits, the function returns true.
 * That means the function returns true if
 * \code
 *      limits->sample_rate_Hz.min <= metrics->sample_rate_Hz <= limits->sample_rate_Hz.max
 * \endcode
 * and similar for all other other struct members of metrics. Otherwise the function returns false.
 *
 * @param [in]     avian     Avian device handle
 * @param [in]     metrics   Pointer to metrics structure.
 * @param [out]    limits    Pointer to limits structure.
 * @retval         true      if all members of metrics are within the corresponding min and max values of limits
 * @retval         false     otherwise
 */
IFX_DLL_PUBLIC
bool ifx_avian_metrics_get_limits(const ifx_Avian_Device_t* avian, const ifx_Avian_Metrics_t* metrics, ifx_Avian_Metrics_Limits_t* limits);

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif /* IFX_AVIAN_METRICS_H */
