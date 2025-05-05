/* ===========================================================================
** Copyright (C) 2021 Infineon Technologies AG
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
 * @internal
 * @file DeviceCalc.h
 *
 * @brief Internal functions to compute radar parameters.
 */

#ifndef IFX_RADAR_INTERNAL_DEVICE_CALC_H
#define IFX_RADAR_INTERNAL_DEVICE_CALC_H

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
*/

#include "ifxBase/Types.h"

/*
==============================================================================
   2. DEFINITIONS
==============================================================================
*/

typedef struct {
    double pre_chirp_delay;        /**< pre-chirp-delay in seconds (T_START) */
    double post_chirp_delay;       /**< post-chirp-delay in seconds (T_END) */
    double pa_delay;               /**< PA delay in seconds (T_PAEN) */
    double adc_delay;              /**< ADC delay in seconds (T_SSTART) */
    double wake_up_time;           /**< wakeup time in seconds (T_WU) */
    double pll_settle_time_coarse; /**< PLL settle time coarse in seconds (T_INIT0) */
    double pll_settle_time_fine;   /**< PLL settle time fine in seconds (T_INIT1) */
    double chirp_end_delay;        /**< chirp end delay in seconds */
} ifx_Avian_Calc_Delays_t;

typedef struct ifx_Avian_Calc_s ifx_Avian_Calc_t;

/*
==============================================================================
   3. TYPES
==============================================================================
*/

// the values must match the definitions in EndpointRadarBGT60TRxx.h
typedef enum {
    IFX_AVIAN_POWER_MODE_ACTIVE = 0, // EP_RADAR_BGT60TRXX_POW_STAY_ACTIVE
    IFX_AVIAN_POWER_MODE_IDLE = 1, // EP_RADAR_BGT60TRXX_POW_GO_IDLE
    IFX_AVIAN_POWER_MODE_DEEP_SLEEP_CONTINUE = 3, // EP_RADAR_BGT60TRXX_POW_GO_DEEP_SLEEP_CONTINUE
} ifx_avian_power_mode_t;

// None 1, Azimuth, Elevation 2, AZ& EL 3
typedef enum {
    IFX_AVIAN_ANGLE_MEASUREMENT_NONE = 0,
    IFX_AVIAN_ANGLE_MEASUREMENT_AZIMUTH,
    IFX_AVIAN_ANGLE_MEASUREMENT_ELEVATION,
    IFX_AVIAN_ANGLE_MEASUREMENT_AZIMUTH_ELEVATION,
    IFX_AVIAN_ANGLE_MEASUREMENT_UNKNOWN_ANGLE
} ifx_avian_angle_measurement_t;

/*
==============================================================================
   4. FUNCTION PROTOTYPES
==============================================================================
*/

/**
 * @brief Create instance for device calculator
 *
 * The calculator will use the device delays given by delays. If the pointer
 * delays is NULL, default values for the delays are used.
 *
 * To destroy the instance and free the allocated memory use
 * \ref ifx_avian_calc_destroy.
 *
 * @param [in]  delays     pointer to struct with device delays
 * @retval      device calculator handle
 */
IFX_DLL_HIDDEN
ifx_Avian_Calc_t* ifx_avian_calc_create(const ifx_Avian_Calc_Delays_t* delays);

/**
 * @brief Destroy instance for device calculator
 *
 * @param [in,out]  handle  device calculator handle
 */
IFX_DLL_HIDDEN
void ifx_avian_calc_destroy(ifx_Avian_Calc_t* handle);

/**
 * @brief Set speed measurement
 *
 * speed_measurement is a bool corresponding to whether to measure speed or
 * not. Speed measurements require multiple chirps in a frame. Without
 * speed measurements, a single chirp is sufficient.
 *
 * By default speed_measurement is set to true.
 *
 * @param [in]  handle              device calculator handle
 * @param [in]  speed_measurement   flag indicating if speeds should be measured
 */
IFX_DLL_HIDDEN
void ifx_avian_calc_set_speed_measurement(ifx_Avian_Calc_t* handle, bool speed_measurement);

/**
 * @brief Set powers of two
 *
 * If power_of_twos is true, num_samples_per_chirp and num_chirps_per_frame
 * will be a power of 2, i.e., will are given by 2**N where N is a positive
 * integer.
 *
 * By default power_of_twos is set to true.
 *
 * @param [in]  handle          device calculator handle
 * @param [in]  power_of_twos   flag (see description above)
 */
IFX_DLL_HIDDEN
void ifx_avian_calc_set_power_of_twos(ifx_Avian_Calc_t* handle, bool power_of_twos);

/**
 * @brief Computes bounds for range resolution
 *
 * The lower boundary for range_resolution will be written to lower_bound, the upper
 * boundary will be written to upper_bound.
 *
 * @param [in]  handle          device calculator handle
 * @param [out] lower_bound     lower bound for max_range (in m)
 * @param [out] upper_bound     upper bound for max_range (in m)
 *
 */
IFX_DLL_HIDDEN
void ifx_avian_calc_range_resolution_bounds(const ifx_Avian_Calc_t* handle, double* lower_bound, double* upper_bound);

/**
 * @brief Computes bounds for maximum range
 *
 * Maximum range specifies the maximum distance from the radar sensor up to
 * which objects can be still detected.
 *
 * The lower boundary for max_range will be written to lower_bound, the upper
 * boundary will be written to upper_bound.
 *
 * @param [in]  handle              device calculator handle
 * @param [in]  range_resolution    input range resolution (in m)
 * @param [out] lower_bound         lower bound for max_range (in m)
 * @param [out] upper_bound         upper bound for max_range (in m)
 *
 */
IFX_DLL_HIDDEN
void ifx_avian_calc_max_range_bounds(const ifx_Avian_Calc_t* handle, double range_resolution, double* lower_bound, double* upper_bound);

/**
 * @brief Computes bounds for ADC sampling rate
 *
 * The ADC sampling rate defines how fast the samples are acquired and has
 * therefore a big influence on the sampling time and the chirp time. See
 * also \ref and ifx_avian_calc_sampling_time \ref ifx_avian_calc_chirp_time.
 *
 * Note that the BGT60TR13C has an anti-aliasing filter with a cut-off
 * frequency of 500 kHz. Hence, one will loose information when sampling
 * faster than 1 MHz.
 *
 * The lower boundary for max_range will be written to lower_bound, the upper
 * boundary will be written to upper_bound.
 *
 * @param [in]  handle          device calculator handle
 * @param [out] lower_bound     lower bound for max_range (in Hz)
 * @param [out] upper_bound     upper bound for max_range (in Hz)
 *
 */
IFX_DLL_HIDDEN
void ifx_avian_calc_sample_rate_bounds(const ifx_Avian_Calc_t* handle, uint32_t* lower_bound, uint32_t* upper_bound);

/**
 * @brief Computes number of samples per chirp
 *
 * If power_of_twos is set using \ref ifx_avian_calc_set_power_of_twos the
 * returned number of chirps will be a power of 2.
 *
 * @param [in]  handle              device calculator handle
 * @param [in]  max_range           maximum range (in m)
 * @param [in]  range_resolution    range resolution (in m)
 * @return      number of samples per chirp
 *
 */
IFX_DLL_HIDDEN
uint32_t ifx_avian_calc_num_samples_per_chirp(const ifx_Avian_Calc_t* handle, double max_range, double range_resolution);

/**
 * @brief Computes theoretical maximum range
 *
 * Due to rounding-up up of the number of samples per chirp, the computed
 * maximum range can be slightly higher than the configured maximum range.
 *
 * See also \ref ifx_avian_calc_num_samples_per_chirp.
 *
 * @param [in]  handle                      device calculator handle
 * @param [in]  actual_range_resolution     actual range resolution (in m)
 * @param [in]  num_samples_per_chirp       number of samples per chirp
 * @return      theoretical maximum range (in m)
 *
 */
IFX_DLL_HIDDEN
double ifx_avian_calc_theoretical_max_range(const ifx_Avian_Calc_t* handle, double range_resolution, uint32_t num_samples_per_chirp);

/**
 * @brief Computes the sampling time
 *
 * The sampling time is the time during which the ADC acquires data.
 *
 * See also \ref ifx_avian_calc_num_samples_per_chirp.
 *
 * @param [in]  handle                  device calculator handle
 * @param [in]  num_samples_per_chirp   number of samples per chirp
 * @param [in]  sample_rate             ADC sample rate (in Hz)
 * @return      sampling time (in s)
 *
 */
IFX_DLL_HIDDEN
double ifx_avian_calc_sampling_time(const ifx_Avian_Calc_t* handle, uint32_t num_samples_per_chirp, uint32_t sample_rate);

/**
 * @brief Computes the chirp time
 *
 * The chirp time is the total chirp time consisting of the sampling time and
 * the sampling delay.
 *
 * See also \ref ifx_avian_calc_sampling_time.
 *
 * @param [in]  handle              device calculator handle
 * @param [in]  sampling_time       sampling time (in s)
 * @return      chirp time (in s)
 *
 */
IFX_DLL_HIDDEN
double ifx_avian_calc_chirp_time(const ifx_Avian_Calc_t* handle, double sampling_time);

/**
 * @brief Computes the minimum chirp repetition time with interchirp power mode as active
 *
 * The minimum chirp repetition time consists of the sampling time, pre and post chirp delays and minimum shape end delay.
 * Power mode is also set to Active.
 *
 * @param [in]  handle              device calculator handle
 * @param [in]  sampling_time       sampling time (in s)
 * @return      minimum chirp repetition time (in s)
 *
 */
IFX_DLL_HIDDEN
double ifx_avian_calc_min_chirp_repetition_time(const ifx_Avian_Calc_t* handle, double sampling_time);

/**
 * @brief Computes the minimum frame repetition time
 *
 * The minimum frame repetition time consists of the current chirp repetition time, number of chirps per frame and minimum frame end delay of 15ns.
 *
 * @param [in]  handle                 device calculator handle
 * @param [in]  pulse_repetition_time  minimum pulse repetition time (in s)
 * @param [in]  num_chirps_per_frame   Number of chirps per frame
 * @return      minimum frame repetition time (in s)
 *
 */
IFX_DLL_HIDDEN
double ifx_avian_calc_min_frame_repetition_time(const ifx_Avian_Calc_t* handle, double pulse_repetition_time, uint32_t num_chirps_per_frame);

/**
 * @brief Computes the sampling bandwidth
 *
 * The sampling bandwidth is the bandwidth used for sampling.
 *
 * @param [in]  handle                      device calculator handle
 * @param [in]  range_resolution            range resolution (in m)
 * @return      sampling bandwidth (in Hz)
 *
 */
IFX_DLL_HIDDEN
double ifx_avian_calc_sampling_bandwidth(const ifx_Avian_Calc_t* handle, double range_resolution);

/**
 * @brief Computes the total bandwidth
 *
 * The total bandwidth is the total bandwidth of a chirp including the sampling
 * delay.
 *
 * See also \ref ifx_avian_calc_sampling_time, \ref ifx_avian_calc_chirp_time,
 * \ref ifx_avian_calc_sampling_bandwidth.
 *
 * @param [in]  handle              device calculator handle
 * @param [in]  sampling_time       sampling time (in s)
 * @param [in]  chirp_time          chirp time (in s)
 * @param [in]  sampling_bandwidth  sampling bandwidth (in Hz)
 * @return total bandwidth (in Hz)
 *
 */
IFX_DLL_HIDDEN
double ifx_avian_calc_total_bandwidth(const ifx_Avian_Calc_t* handle, double sampling_time, double chirp_time, double sampling_bandwidth);

/**
 * @brief Computes the center frequency
 *
 * The center frequency of the chirp.
 *
 * If the bandwidth is less than or equal to 500MHz the 61.25GHz is used as
 * center frequency. With this choice the minimum and maximum frequencies
 * are within the ISM radio band.
 *
 * If the bandwidth exceeds 500MHz the center frequency is 59.5GHz. The minimum
 * and/or maximum frequency will be outside of the ISM radio band.
 *
 * See also \ref ifx_avian_calc_total_bandwidth.
 *
 * @param [in]  handle              device calculator handle
 * @param [in]  total_bandwidth     total bandwidth (in Hz)
 * @return center frequency (in Hz)
 *
 */
IFX_DLL_HIDDEN
double ifx_avian_calc_center_frequency(const ifx_Avian_Calc_t* handle, double total_bandwidth);

/**
 * @brief Computes the wavelength
 *
 * Computes the wavelength \f$\lambda = c/f\f$ corresponding to the center
 * frequency \f$f\f$.
 *
 * See also \ref ifx_avian_calc_center_frequency.
 *
 * @param [in]  handle              device calculator handle
 * @param [in]  center_frequency    center frequency (in Hz)
 * @return wavelength (in m)
 *
 */
IFX_DLL_HIDDEN
double ifx_avian_calc_wavelength(const ifx_Avian_Calc_t* handle, double center_frequency);

/**
 * @brief Computes the bounds for the chirp frequency
 *
 * Computes the minimum and maximum frequency of a chirp.
 *
 * See also \ref ifx_avian_calc_center_frequency,
 * \ref ifx_avian_calc_total_bandwidth.
 *
 * @param [in]  handle              device calculator handle
 * @param [in]  total_bandwidth     total bandwidth (in Hz)
 * @param [in]  center_frequency    center frequency (in Hz)
 * @param [out] lower_bound         minimum frequency of ramp (in Hz)
 * @param [out] upper_bound         maximum frequency of ramp (in Hz)
 *
 */
IFX_DLL_HIDDEN
void ifx_avian_calc_chirp_frequency_bounds(const ifx_Avian_Calc_t* handle, double total_bandwidth, double center_frequency, double* lower_bound, double* upper_bound);

/**
 * @brief Computes the bounds for the sampling frequency
 *
 * Computes the minimum and maximum frequency of a chirp.
 *
 * See also \ref ifx_avian_calc_sampling_bandwidth
 * \ref ifx_avian_calc_chirp_frequency_bounds.
 *
 * @param [in]  handle                device calculator handle
 * @param [in]  sampling_bandwidth    sampling bandwidth (in Hz)
 * @param [in]  end_chirp_frequency   end chirp frequency (in Hz)
 * @param [out] lower_bound           lower bound of sampling frequency (in Hz)
 * @param [out] upper_bound           upper bound of sampling frequency (in Hz)
 *
 */
IFX_DLL_HIDDEN
void ifx_avian_calc_sampling_frequency_bounds(const ifx_Avian_Calc_t* handle, double sampling_bandwidth, double end_chirp_frequency, double* lower_bound, double* upper_bound);

/**
 * @brief Computes the bounds for the maximum speed
 *
 * Maximum speed is the maximum speed that one wants to be able to measure
 * with a micro-Doppler measurement. (Required only if speed measurement is
 * required.)
 *
 * See also \ref ifx_avian_calc_wavelength,
 * \ref ifx_avian_calc_sampling_time.
 *
 * @param [in]  handle              device calculator handle
 * @param [in]  wavelength          wavelength (in m)
 * @param [in]  sampling_time       sampling time (in s)
 * @param [out] lower_bound         lower bound of maximum speed (in m/s)
 * @param [out] upper_bound         upper bound of maximum speed (in m/s)
 *
 */
IFX_DLL_HIDDEN
void ifx_avian_calc_max_speed_bounds(const ifx_Avian_Calc_t* handle, double wavelength, double sampling_time, double* lower_bound, double* upper_bound);

/**
 * @brief Computes the bounds for the speed resolution
 *
 * Speed resolution is the desired speed resolution that one wants to achieve
 * with micro-Doppler measurement. That means what velocity difference one
 * can detect for two targets at the same distance from the radar.
 *
 * See also \ref ifx_avian_calc_max_speed_bounds.
 *
 * @param [in]  handle              device calculator handle
 * @param [in]  max_speed           maximum speed one wants to measure (in m/s)
 * @param [out] lower_bound         lower bound of speed resolution (in m/s)
 * @param [out] upper_bound         upper bound of speed resolution (in m/s)
 *
 */
IFX_DLL_HIDDEN
void ifx_avian_calc_speed_resolution_bounds(const ifx_Avian_Calc_t* handle, double max_speed, double* lower_bound, double* upper_bound);

/**
 * @brief Computes the chirp repetition time
 *
 * The chirp repetition time is the time between the start of two
 * consecutive chirps within a frame. It is also known as pulse repetition time
 * (PRT).
 *
 * See also \ref ifx_avian_calc_max_speed_bounds,
 * \ref ifx_avian_calc_wavelength.
 *
 * @param [in]  handle              device calculator handle
 * @param [in]  max_speed           maximum speed one wants to measure (in m/s)
 * @param [in]  wavelength          wavelength (in m)
 * @return chirp repetition time (in s)
 *
 */
IFX_DLL_HIDDEN
double ifx_avian_calc_chirp_repetition_time(const ifx_Avian_Calc_t* handle, double max_speed, double wavelength);

/**
 * @brief Computes the number of chirps
 *
 * The number of chirps is the resulting number of chirps in the frame.
 *
 * See also \ref ifx_avian_calc_max_speed_bounds,
 * \ref ifx_avian_calc_speed_resolution_bounds.
 *
 * @param [in]  handle              device calculator handle
 * @param [in]  max_speed           maximum speed (in m/s)
 * @param [in]  speed_resolution    speed resolution (in m/s)
 * @return number of chirps
 *
 */
IFX_DLL_HIDDEN
uint32_t ifx_avian_calc_num_chirps_per_frame(const ifx_Avian_Calc_t* handle, double max_speed, double speed_resolution);

/**
 * @brief Computes the theoretical speed resolution
 *
 * The theoretical speed resolution is the calculated speed resolution for the
 * given number of chirps. (This can vary from the configured value because
 * of the power_of_twos setting, see \ref ifx_avian_calc_set_power_of_twos.)
 *
 * See also \ref ifx_avian_calc_max_speed_bounds,
 * \ref ifx_avian_calc_num_chirps_per_frame.
 *
 * @param [in]  handle                  device calculator handle
 * @param [in]  max_speed               maximum speed (in m/s)
 * @param [in]  num_chirps_per_frame    number of chirps
 * @return theoretical speed resolution (in m/s)
 *
 */
IFX_DLL_HIDDEN
double ifx_avian_calc_theoretical_speed_resolution(const ifx_Avian_Calc_t* handle, double max_speed, uint32_t num_chirps_per_frame);

/**
 * @brief Computes the shape end delay
 *
 * The shape end delay (T_SED) is the required delay after the end of a chirp
 * to achieve the desired pulse repetition rate.
 *
 * The returned shape end delay might be negative which means that the desired
 * pulse repetition time (PRT) cannot be achieved. The best you can do in this
 * case is to set shape end delay to 0.
 *
 * The power mode after the end of the shape is written to shape_end_power_mode
 * if shape_end_power_mode is not NULL.
 *
 * See also \ref ifx_avian_calc_pulse_repetition_time,
 * \ref ifx_avian_calc_sampling_time.
 *
 * @param [in]  handle                  device calculator handle
 * @param [in]  mimo                    flag indicating if MIMO is activated (ATR24C only)
 * @param [in]  sampling_time           sampling time (in s)
 * @param [in]  pulse_repetition_time   pulse repetition time (PRT; in s)
 * @param [out] shape_end_power_mode    power mode after end of shape
 * @return shape end power delay
 *
 */
IFX_DLL_HIDDEN
double ifx_avian_calc_shape_end_delay(const ifx_Avian_Calc_t* handle, bool mimo, double sampling_time, double pulse_repetition_time, ifx_avian_power_mode_t* shape_end_power_mode);

/**
 * @brief Computes the pulse repetition time from the shape end delay
 *
 * Compute the pulse repetition time (PRT) from the shape end delay (T_SED),
 * the sampling time, and the shape end power mode.
 *
 * See also \ref ifx_avian_calc_pulse_repetition_time,
 * \ref ifx_avian_calc_sampling_time, \ref ifx_avian_calc_shape_end_delay.
 *
 * @param [in]  handle                  device calculator handle
 * @param [in]  mimo                    flag indicating if MIMO is activated (ATR24C only)
 * @param [in]  shape_end_delay         shape end delay (in s; T_SED)
 * @param [in]  sampling_time           sampling time (in s)
 * @param [in]  shape_end_power_mode    power mode after shape
 * @return pulse repetition time
 *
 */
IFX_DLL_HIDDEN
double ifx_avian_calc_pulse_repetition_time_from_shape_end_delay(const ifx_Avian_Calc_t* handle, bool mimo, double shape_end_delay, double sampling_time, ifx_avian_power_mode_t shape_end_power_mode);

/**
 * @brief Computes range bin frequency
 *
 * See also \ref ifx_avian_calc_num_samples_per_chirp.
 *
 * @param [in]  handle                  device calculator handle
 * @param [in]  sample_rate             ADC sample rate (in Hz)
 * @param [in]  num_samples_per_chirp   number of samples per chirp
 * @return range bin frequency (in Hz)
 *
 */
IFX_DLL_HIDDEN
double ifx_avian_calc_range_bin_frequency(const ifx_Avian_Calc_t* handle, uint32_t sample_rate, uint32_t num_samples_per_chirp);

/**
 * @brief Computes the doppler shift corresponding to the maximum speed
 *
 * See also \ref ifx_avian_calc_max_speed_bounds, \ref ifx_avian_calc_wavelength.
 *
 * @param [in]  handle      device calculator handle
 * @param [in]  wavelength  wavelength (in m)
 * @param [in]  max_speed   maximum speed (in m/s)
 * @return velocity of the maximum doppler shift (in m/s)
 *
 */
IFX_DLL_HIDDEN
double ifx_avian_calc_doppler_shift_vmax(const ifx_Avian_Calc_t* handle, double wavelength, double max_speed);

/**
 * @brief Computes the maximum doppler shift in range bins
 *
 * Maximum doppler frequency: To avoid range migration, one can see here how
 * many range bins (= beat frequency or FFT bins) the maximum speeds
 * corresponds to.		
 *
 * See also \ref ifx_avian_calc_range_bin_frequency,
 * \ref ifx_avian_calc_doppler_shift_vmax.
 *
 * @param [in]  handle               device calculator handle
 * @param [in]  range_bin_frequency  wavelength (in m)
 * @param [in]  doppler_shift_vmax   maximum speed (in m/s)
 * @return velocity of the maximum doppler shift (in m/s)
 *
 */
IFX_DLL_HIDDEN
double ifx_avian_calc_max_doppler_shift(const ifx_Avian_Calc_t* handle, double range_bin_frequency, double doppler_shift_vmax);

/**
 * @brief Computes the bounds for the frame rate
 *
 * Speed resolution is the desired speed resolution that one wants to achieve
 * with micro-Doppler measurement. That means what velocity difference one
 * can detect for two targets at the same distance from the radar.
 *
 * See also \ref ifx_avian_calc_chirp_time, \ref ifx_avian_calc_pulse_repetition_time,
 * \ref ifx_avian_calc_num_chirps_per_frame.
 *
 * @param [in]  handle                  device calculator handle
 * @param [in]  sampling_time           sampling time (in s)
 * @param [in]  pulse_repetition_time   pulse repetition time (PRT; in s)
 * @param [in]  num_chirps_per_frame    number of chirps
 * @param [out] lower_bound             lower bound of frame rate (in Hz)
 * @param [out] upper_bound             upper bound of frame rate (in Hz)
 *
 */
IFX_DLL_HIDDEN
void ifx_avian_calc_frame_repetition_time_bounds(const ifx_Avian_Calc_t* handle, double sampling_time, double pulse_repetition_time, uint32_t num_chirps_per_frame, double* lower_bound, double* upper_bound);

/**
 * @brief Computes the frame time
 *
 * The frame time is the inverse of the frame rate, frame_time = 1/frame_rate.
 *
 * See also \ref ifx_avian_calc_frame_rate_bounds.
 *
 * @param [in]  handle      device calculator handle
 * @param [in]  frame_rate  frame rate (in Hz)
 * @return frame time (in s)
 *
 */
IFX_DLL_HIDDEN
double ifx_avian_calc_frame_repetition_time(const ifx_Avian_Calc_t* handle, double frame_rate);

/**
 * @brief Computes the frame end delay
 *
 * The frame end delay (T_FED) is the required delay after the end of a frame
 * to achieve the desired pulse repetition rate (PRT).
 *
 * The returned frame end delay might be negative which means that the desired
 * frame time cannot be achieved. The best you can do in this case is to set
 * frame end delay to 0.
 *
 * See also \ref ifx_avian_calc_sampling_time,
 * \ref ifx_avian_calc_pulse_repetition_time,
 * \ref ifx_avian_calc_num_chirps_per_frame, \ref ifx_avian_calc_frame_time.
 *
 * @param [in]  handle                  device calculator handle
 * @param [in]  sampling_time           sampling time (in s)
 * @param [in]  pulse_repetition_time   pulse repetition time (PRT; in s)
 * @param [in]  num_chirps_per_frame    number of chirps per frame
 * @param [in]  frame_time              frame time (in s)
 * @param [out] frame_end_power_mode    power mode after end of frame
 * @return frame end power time (in s)
 */
IFX_DLL_HIDDEN
double ifx_avian_calc_frame_end_delay(const ifx_Avian_Calc_t* handle, double sampling_time, double pulse_repetition_time, uint32_t num_chirps_per_frame, double frame_time, ifx_avian_power_mode_t* frame_end_power_mode);

/**
 * @brief Computes the frame time from the frame end delay
 *
 * @param [in]  handle                  device calculator handle
 * @param [in]  frame_end_delay         frame end time (in s)
 * @param [in]  sampling_time           sampling time (in s)
 * @param [in]  pulse_repetition_time   pulse repetition time (PRT; in s)
 * @param [in]  num_chirps_per_frame    number of chirps per frame
 * @param [in]  frame_time              frame time (in s)
 * @param [out] frame_end_power_mode    power mode after end of frame
 * @return frame end power time (in s)
 */
IFX_DLL_HIDDEN
double ifx_avian_calc_frame_repetition_time_from_frame_end_delay(const ifx_Avian_Calc_t* handle, double frame_end_delay, double sampling_time, double pulse_repetition_time, uint32_t num_chirps_per_frame, ifx_avian_power_mode_t frame_end_power_mode);

/**
 * @brief Computes the time spent in deep sleep mode
 *
 * See also \ref ifx_avian_calc_num_chirps_per_frame,
 * \ref ifx_avian_calc_shape_end_power_mode, \ref ifx_avian_calc_shape_end_delay,
 * \ref ifx_avian_calc_frame_end_power_mode, \ref ifx_avian_calc_frame_end_delay.
 *
 * @param [in]  handle                  device calculator handle
 * @param [in]  num_chirps_per_frame    number of chirps per frame
 * @param [in]  shape_end_power_mode    shape end power mode
 * @param [in]  shape_end_delay         shape end delay (in s)
 * @param [in]  frame_end_power_mode    frame end power mode
 * @param [in]  frame_end_delay         frame end delay (in s)
 * @return time spent in deep sleep mode (in s)
 *
 */
IFX_DLL_HIDDEN
double ifx_avian_calc_time_DS(const ifx_Avian_Calc_t* handle, uint32_t num_chirps_per_frame, ifx_avian_power_mode_t shape_end_power_mode, double shape_end_delay, ifx_avian_power_mode_t frame_end_power_mode, double frame_end_delay);

/**
 * @brief Computes the time spent in idle mode
 *
 * See also \ref ifx_avian_calc_num_chirps_per_frame,
 * \ref ifx_avian_calc_shape_end_power_mode, \ref ifx_avian_calc_shape_end_delay,
 * \ref ifx_avian_calc_frame_end_power_mode, \ref ifx_avian_calc_frame_end_delay.
 *
 * @param [in]  handle                  device calculator handle
 * @param [in]  num_chirps_per_frame    number of chirps per frame
 * @param [in]  shape_end_power_mode    shape end power mode
 * @param [in]  shape_end_delay         shape end delay (in s)
 * @param [in]  frame_end_power_mode    frame end power mode
 * @param [in]  frame_end_delay         frame end delay (in s)
 * @return time spent in idle mode (in s)
 *
 */
IFX_DLL_HIDDEN
double ifx_avian_calc_time_idle(const ifx_Avian_Calc_t* handle, uint32_t num_chirps_per_frame, ifx_avian_power_mode_t shape_end_power_mode, double shape_end_delay, ifx_avian_power_mode_t frame_end_power_mode, double frame_end_delay);

/**
 * @brief Computes the time spent between two chirps
 *
 * See also \ref ifx_avian_calc_num_chirps_per_frame,
 * \ref ifx_avian_calc_shape_end_power_mode, \ref ifx_avian_calc_shape_end_delay,
 * \ref ifx_avian_calc_frame_end_power_mode, \ref ifx_avian_calc_frame_end_delay.
 *
 * @param [in]  handle                  device calculator handle
 * @param [in]  num_chirps_per_frame    number of chirps per frame
 * @param [in]  shape_end_power_mode    shape end power mode
 * @param [in]  shape_end_delay         shape end delay (in s)
 * @param [in]  frame_end_power_mode    frame end power mode
 * @param [in]  frame_end_delay         frame end delay (in s)
 * @return time spent between two chirps (in s)
 *
 */
IFX_DLL_HIDDEN
double ifx_avian_calc_time_interchirp(const ifx_Avian_Calc_t* handle, uint32_t num_chirps_per_frame, ifx_avian_power_mode_t shape_end_power_mode, double shape_end_delay, ifx_avian_power_mode_t frame_end_power_mode, double frame_end_delay);

/**
 * @brief Computes the time spent sampling
 *
 * See also \ref ifx_avian_calc_sampling_time,
 * \ref ifx_avian_calc_num_chirps_per_frame.
 *
 * @param [in]  handle                  device calculator handle
 * @param [in]  sampling_time           sampling time (in s)
 * @param [in]  num_chirps_per_frame    number of chirps per frame
 * @return time spent sampling (in s)
 *
 */
IFX_DLL_HIDDEN
double ifx_avian_calc_time_sampling(const ifx_Avian_Calc_t* handle, double sampling_time, uint32_t num_chirps_per_frame);

/**
 * @brief Computes the duty cycle sampling
 *
 * Duty cycle sampling is the ratio of the sampling time (with PA on to emit
 * radar signals) over the total frame time.
 *
 * See also \ref ifx_avian_calc_frame_time, \ref ifx_avian_calc_time_sampling.
 * \ref ifx_avian_calc_num_chirps_per_frame.
 *
 * @param [in]  handle           device calculator handle
 * @param [in]  frame_time       frame time (in s)
 * @param [in]  time_sampling    time sampling (in s)
 * @return duty cycle sampling
 *
 */
IFX_DLL_HIDDEN
double ifx_avian_calc_duty_cycle_sampling(const ifx_Avian_Calc_t* handle, double frame_time, double time_sampling);

/**
 * @brief Computes the duty cycle active sampling
 *
 * Duty cycle active is the ratio of the active time (with the VCO activated)
 * over the total frame time.
 *
 * See also \ref ifx_avian_calc_frame_time, \ref ifx_avian_calc_time_interchirp
 * \ref ifx_avian_calc_time_sampling.
 * \ref ifx_avian_calc_num_chirps_per_frame.
 *
 * @param [in]  handle           device calculator handle
 * @param [in]  frame_time       frame time (in s)
 * @param [in]  time_sampling    time sampling (in s)
 * @return duty cycle sampling active
 *
 */
IFX_DLL_HIDDEN
double ifx_avian_calc_duty_cycle_active(const ifx_Avian_Calc_t* handle, double frame_time, double time_interchirp, double time_sampling);

/**
 * @brief Computes the power spent in deep sleep
 *
 * @param [in]  handle  device calculator handle
 * @return power spent in deep sleep (in W)
 *
 */
IFX_DLL_HIDDEN
double ifx_avian_calc_power_DS(const ifx_Avian_Calc_t* handle);

/**
 * @brief Computes the power spent in idle mode
 *
 * @param [in]  handle  device calculator handle
 * @return power spent in idle mode (in W)
 *
 */
IFX_DLL_HIDDEN
double ifx_avian_calc_power_idle(const ifx_Avian_Calc_t* handle);

/**
 * @brief Computes the power spent between chirps
 *
 * @param [in]  handle              device calculator handle
 * @param [in] angle_measurement    angle measurement
 * @return power spent between chirps (in W)
 *
 */
IFX_DLL_HIDDEN
double ifx_avian_calc_power_interchirp(const ifx_Avian_Calc_t* handle, ifx_avian_angle_measurement_t angle_measurement);

/**
 * @brief Computes the power spent while sampling
 *
 * @param [in] handle               device calculator handle
 * @param [in] angle_measurement    angle measurement
 * @param [in] tx_power_setting     TX power setting
 * @return power spent while sampling (in W)
 *
 */
IFX_DLL_HIDDEN
double ifx_avian_calc_power_sampling(const ifx_Avian_Calc_t* handle, ifx_avian_angle_measurement_t angle_measurement, uint8_t tx_power_setting);

/**
 * @brief Computes the average power consumption
 *
 * The average power is the average power of the configured frame.
 *
 * See also \ref ifx_avian_calc_num_chirps_per_frame,
 * \ref ifx_avian_calc_shape_end_power_mode, \ref ifx_avian_calc_shape_end_delay,
 * \ref ifx_avian_calc_frame_end_power_mode, \ref ifx_avian_calc_frame_end_delay,
 * \ref ifx_avian_calc_frame_time, \ref ifx_avian_calc_sampling_time.
 *
 * @param [in] handle                   device calculator handle
 * @param [in] num_chirps_per_frame     number of chirps per frame
 * @param [in] shape_end_power_mode     shape end power power mode
 * @param [in] shape_end_delay          shape end delay
 * @param [in] frame_end_power_mode     frame end power mode
 * @param [in] frame_end_delay          frame end delay
 * @param [in angle_measurement         angle measurement
 * @param [in] tx_power                 TX power
 * @param [in] frame_time               frame time
 * @param [in] sampling_time            sampling time
 *
 * @return average power consumption (in W)
 *
 */
IFX_DLL_HIDDEN
double ifx_avian_calc_average_power(const ifx_Avian_Calc_t* handle, uint32_t num_chirps_per_frame, ifx_avian_power_mode_t shape_end_power_mode, double shape_end_delay, ifx_avian_power_mode_t frame_end_power_mode, double frame_end_delay, ifx_avian_angle_measurement_t angle_measurement, uint8_t tx_power, double frame_time, double sampling_time);

/**
 * @brief Computes the minimum number of antennas required
 *
 * @param [in] handle                   device calculator handle
 * @param [in] angle_measurement        angle measurement
 * @return minimum number of antennas required
 *
 */
IFX_DLL_HIDDEN
uint8_t ifx_avian_calc_number_of_antennas(const ifx_Avian_Calc_t* handle, ifx_avian_angle_measurement_t angle_measurement);


#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif /* IFX_RADAR_INTERNAL_DEVICE_CALC_H */
