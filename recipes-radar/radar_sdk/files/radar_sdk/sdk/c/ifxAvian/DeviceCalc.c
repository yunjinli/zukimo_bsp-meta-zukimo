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

// TODO: convert to C++

/**
 * This is the C version of the Excel sheet to compute range doppler
 * quantities for the BGT60TR13C (RangeDopplerWithBGT60TR13C_x2.xlsx in the
 * directory tools/Excel2Python-Converter).
 *
 * All quantities use the standard SI units meter (m) and second (s) or derived
 * quantities like m/s, Hz = 1/s or W.
 *
 * This code does not contain any rounding functions as in the Excel sheet.
 * Matthias Brandl, author of the original Excel sheet, confirmed that the
 * rounding functions in the Excel sheet only have cosmetic purposes.
 */

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
*/

#include <math.h>

#include "ifxBase/Defines.h"
#include "ifxBase/Math.h"
#include "ifxBase/Mem.h"
#include "ifxBase/Error.h"

#include "ifxAvian/internal/DeviceCalc.h"

#include "DeviceConfig.h"

/*
==============================================================================
   2. LOCAL DEFINITIONS
==============================================================================
*/

#define IGNORE(x) (void)(x)

/* The timing values are taken from sensing gui. An even better approach would
 * be to avoid the constants in the code entirely and use the values we get
 * from the device (via commlib).
 *
 * All delays are in seconds, frequencies are in Hz=1/s. The name names in
 * parentheses are the sensing gui names of the constants.s
 */

/* default values of delays */
#define DEFAULT_PRE_CHIRP_TIME_IDLE   53.2125e-6  /* T_INIT0 + T_INIT1 + T_START */
#define DEFAULT_IDLE_TIME_AFTER_DS    998.5375e-6 /* T_WU */
#define DEFAULT_SAMPLING_DELAY        4.9875e-6   /* T_PAEN + T_SSTART */
#define DEFAULT_POST_CHIRP_DELAY      1.5875e-6   /* T_END + T_EDU + T_EDD + T_RAMP_DOWN_FAST */
#define DEFAULT_PRE_CHIRP_TIME_ACTIVE 1.125e-6    /* T_START */

/* Minimum Frequency (Hz) */
#define MIN_FREQUENCY     58.1e9 // 100MHz gap for rounding

/* Maximum Frequency (Hz) */
#define MAX_FREQUENCY     62.9e9 // 100MHz gap for rounding

/* Minimum ISM Frequency (Hz) */
#define MIN_ISM_FREQUENCY 61e9

/* Maximum ISM Frequency (Hz) */
#define MAX_ISM_FREQUENCY 61.5e9

/* Lower limit for configuration of Range Resolution (m). */
#define RANGE_RESOLUTION_LOWER_LIMIT  ((ifx_Float_t)0.03) /* 3cm */

/* Upper limit for configuration of Range Resolution (m). */
#define RANGE_RESOLUTION_UPPER_LIMIT  ((ifx_Float_t)0.5) /* 50cm */

/* Upper limit for configuration of Maximum Range (m). */
#define MAX_RANGE_LOWER_LIMIT  ((ifx_Float_t)0.1)
#define MAX_RANGE_UPPER_LIMIT  ((ifx_Float_t)20.0)

/* Lower limit for configuration of Maximum Speed (m/s). */
#define MAX_SPEED_LOWER_LIMIT  ((ifx_Float_t)0.5)
#define MAX_SPEED_UPPER_LIMIT  ((ifx_Float_t)20)

/* Lower limit for configuration of Speed Resolution (m/s). */
#define SPEED_RESOLUTION_LOWER_LIMIT  ((ifx_Float_t)0.01)

#define MIN_NUM_SAMPLES_PER_CHIRP 16
#define MAX_NUM_SAMPLES_PER_CHIRP 1024

#define MIN_NUM_CHIRPS_PER_FRAME 4
#define MAX_NUM_CHIRPS_PER_FRAME 512

#define MIN_TIME_REPETITION_TIME_S 0.005 // 200Hz
#define MAX_TIME_REPETITION_TIME_S 10 // 0.1Hz

#define MIN_SHAPE_END_DELAY_S       150e-9 // 150ns
#define MIN_FRAME_END_DELAY_S       15e-9 // 15ns

struct ifx_Avian_Calc_s {
    /* Pre Chirp Time Idle (s) (T_INIT0+T_INIT1+T_START) */
    double pre_chirp_time_idle;

    /* Idle Time after DS (s) (T_WU) */
    double idle_time_after_ds;

    /* Pre Chirp Time DS (s) (pre_chirp_time_idle+idle_time_after_ds) */
    double pre_chirp_time_ds;

    /* Sampling Delay (s) (T_PAEN - T_START + T_SSTART) */
    double sampling_delay;

    /* Post Chirp Delay (s) (T_END+chirp_end_delay) */
    double post_chirp_delay;

    /* Pre Chirp Time Active (s) (T_START) */
    double pre_chirp_time_active;

    /* Total Chirp Delay Idle (s) */
    double total_chirp_delay_idle;

    /* Total Chirp Delay DS (s) */
    double total_chirp_delay_ds;

    /* Total Chirp Delay Active (s) */
    double total_chirp_delay_active;

    /* speed_measurement is a bool corresponding to whether to measure speed or
     * not. Speed measurements require multiple chirps in a frame. Without
     * speed measurements, a single chirp is sufficient */
    bool speed_measurement;

    /* If true num_samples_per_chirp and num_chirps_per_frame will be a power
     * 2, i.e., will are given by 2**N where N is a positive integer. */
    bool power_of_twos;
};

/*
==============================================================================
   7. EXPORTED FUNCTIONS
==============================================================================
*/

ifx_Avian_Calc_t* ifx_avian_calc_create(const ifx_Avian_Calc_Delays_t* delays)
{
    ifx_Avian_Calc_t* h = ifx_mem_alloc(sizeof(struct ifx_Avian_Calc_s));
    IFX_ERR_BRN_MEMALLOC(h);

    if (delays)
    {
        h->pre_chirp_time_idle = delays->pll_settle_time_coarse + delays->pll_settle_time_fine + delays->pre_chirp_delay;
        h->idle_time_after_ds = delays->wake_up_time;
        h->sampling_delay = delays->pa_delay - delays->pre_chirp_delay + delays->adc_delay;
        h->post_chirp_delay = delays->post_chirp_delay + delays->chirp_end_delay; // find a better name here for post_chirp_delay
        h->pre_chirp_time_active = delays->pre_chirp_delay;
    }
    else
    {
        h->pre_chirp_time_idle = DEFAULT_PRE_CHIRP_TIME_IDLE;   /* T_INIT0 + T_INIT1 + T_START */
        h->idle_time_after_ds = DEFAULT_IDLE_TIME_AFTER_DS;     /* T_WU */
        h->sampling_delay = DEFAULT_SAMPLING_DELAY;             /* T_PAEN + T_SSTART */
        h->post_chirp_delay = DEFAULT_POST_CHIRP_DELAY;         /* T_END + T_EDU + T_EDD + T_RAMP_DOWN_FAST */
        h->pre_chirp_time_active = DEFAULT_PRE_CHIRP_TIME_ACTIVE; /* T_START */
    } 

    h->pre_chirp_time_ds = h->pre_chirp_time_idle + h->idle_time_after_ds;
    h->total_chirp_delay_idle = h->pre_chirp_time_idle + h->sampling_delay + h->post_chirp_delay;
    h->total_chirp_delay_ds = h->pre_chirp_time_ds + h->sampling_delay + h->post_chirp_delay;
    h->total_chirp_delay_active = h->pre_chirp_time_active + h->sampling_delay + h->post_chirp_delay;

    h->speed_measurement = true;
    h->power_of_twos = true;

    return h;
}

void ifx_avian_calc_destroy(ifx_Avian_Calc_t* handle)
{
    ifx_mem_free(handle);
}

void ifx_avian_calc_set_speed_measurement(ifx_Avian_Calc_t* handle, bool speed_measurement)
{
    handle->speed_measurement = speed_measurement;
}

void ifx_avian_calc_set_power_of_twos(ifx_Avian_Calc_t* handle, bool power_of_twos)
{
    handle->power_of_twos = power_of_twos;
}


void ifx_avian_calc_range_resolution_bounds(const ifx_Avian_Calc_t* handle, double* lower_bound, double* upper_bound)
{
    IGNORE(handle);
    const double bandwidth = MAX_FREQUENCY - MIN_FREQUENCY;

    /* for our settings the range resolution is between about 3cm and 0.5m */
    *lower_bound = IFX_LIGHT_SPEED_MPS / (2 * bandwidth);
    *upper_bound = RANGE_RESOLUTION_UPPER_LIMIT;
}

//----------------------------------------------------------------------------

void ifx_avian_calc_max_range_bounds(const ifx_Avian_Calc_t* handle, double range_resolution_input, double* lower_bound, double* upper_bound)
{
    IGNORE(handle);

    /* max_range = num_samples_per_chirp * range_resolution / 2
     *
     * The lower (upper) bound is given by setting MIN_NUM_SAMPLES_PER_CHIRP
     * (MAX_NUM_SAMPLES_PER_CHIRP) for num_sampler_per_chip in the equation
     * above.
     * In addition, make sure that the lower bound >= MAX_RANGE_UPPER_LIMIT and
     * the upper bound <= MAX_RANGE_UPPER_LIMIT.
     */
    *lower_bound = MAX(MAX_RANGE_LOWER_LIMIT, MIN_NUM_SAMPLES_PER_CHIRP * range_resolution_input/2);
    *upper_bound = MIN(MAX_RANGE_UPPER_LIMIT, MAX_NUM_SAMPLES_PER_CHIRP * range_resolution_input/2);
}

//----------------------------------------------------------------------------

void ifx_avian_calc_sample_rate_bounds(const ifx_Avian_Calc_t* handle, uint32_t* lower_bound, uint32_t* upper_bound)
{
    IGNORE(handle);
    *lower_bound = IFX_AVIAN_SAMPLE_RATE_HZ_LOWER;
    *upper_bound = IFX_AVIAN_SAMPLE_RATE_HZ_UPPER;
}

//----------------------------------------------------------------------------

uint32_t ifx_avian_calc_num_samples_per_chirp(const ifx_Avian_Calc_t* handle, double max_range, double range_resolution)
{
    const uint32_t ceil_ratio = (uint32_t)(2*ceil(max_range / range_resolution));

    if (ceil_ratio >= MAX_NUM_SAMPLES_PER_CHIRP)
        return MAX_NUM_SAMPLES_PER_CHIRP;
    if (ceil_ratio <= MIN_NUM_SAMPLES_PER_CHIRP)
        return MIN_NUM_SAMPLES_PER_CHIRP;

    if (handle->power_of_twos)
        return ifx_math_round_up_power_of_2_uint32(ceil_ratio);
    else
        return ceil_ratio;
}

//----------------------------------------------------------------------------

double ifx_avian_calc_theoretical_max_range(const ifx_Avian_Calc_t* handle, double range_resolution, uint32_t num_samples_per_chirp)
{
    IGNORE(handle);
    return range_resolution * num_samples_per_chirp / 2.0;
}

//----------------------------------------------------------------------------

double ifx_avian_calc_sampling_time(const ifx_Avian_Calc_t* handle, uint32_t num_samples_per_chirp, uint32_t sample_rate)
{
    IGNORE(handle);
    return (double)num_samples_per_chirp / sample_rate;
}

//----------------------------------------------------------------------------

double ifx_avian_calc_chirp_time(const ifx_Avian_Calc_t* handle, double sampling_time)
{
    return sampling_time + handle->sampling_delay;
}

//----------------------------------------------------------------------------

double ifx_avian_calc_min_chirp_repetition_time(const ifx_Avian_Calc_t* handle, double sampling_time)
{    
    // with minimum T_SED of 25nsec and active power mode between chirps
    return handle->total_chirp_delay_active + sampling_time + MIN_SHAPE_END_DELAY_S;
}

//----------------------------------------------------------------------------

double ifx_avian_calc_min_frame_repetition_time(const ifx_Avian_Calc_t* handle, double pulse_repetition_time, uint32_t num_chirps_per_frame)
{
    // with frame end power mode of deep sleep and continue
    return handle->pre_chirp_time_ds + pulse_repetition_time * num_chirps_per_frame + MIN_FRAME_END_DELAY_S;
}

//----------------------------------------------------------------------------

double ifx_avian_calc_sampling_bandwidth(const ifx_Avian_Calc_t* handle, double range_resolution)
{
    IGNORE(handle);
    return IFX_LIGHT_SPEED_MPS / (2 * range_resolution);
}

//----------------------------------------------------------------------------

double ifx_avian_calc_total_bandwidth(const ifx_Avian_Calc_t* handle, double sampling_time, double chirp_time, double sampling_bandwidth)
{
    IGNORE(handle);
    return sampling_bandwidth * chirp_time / sampling_time;
}

//----------------------------------------------------------------------------

double ifx_avian_calc_center_frequency(const ifx_Avian_Calc_t* handle, double total_bandwidth)
{
    IGNORE(handle);
    if (total_bandwidth > 500e6)
        return (MIN_FREQUENCY + MAX_FREQUENCY) / 2;
    else
        return (MIN_ISM_FREQUENCY + MAX_ISM_FREQUENCY) / 2;
}

//----------------------------------------------------------------------------

double ifx_avian_calc_wavelength(const ifx_Avian_Calc_t* handle, double center_frequency)
{
    IGNORE(handle);
    return IFX_LIGHT_SPEED_MPS / center_frequency;
}

//----------------------------------------------------------------------------

void ifx_avian_calc_chirp_frequency_bounds(const ifx_Avian_Calc_t* handle, double total_bandwidth, double center_frequency, double* lower_bound, double* upper_bound)
{
    IGNORE(handle);
    *lower_bound = center_frequency - total_bandwidth / 2;
    *upper_bound = center_frequency + total_bandwidth / 2;
}

//----------------------------------------------------------------------------

void ifx_avian_calc_sampling_frequency_bounds(const ifx_Avian_Calc_t* handle, double sampling_bandwidth, double end_chirp_frequency, double* lower_bound, double* upper_bound)
{
    IGNORE(handle);
    *lower_bound = end_chirp_frequency - sampling_bandwidth;
    *upper_bound = end_chirp_frequency;
}

//----------------------------------------------------------------------------

void ifx_avian_calc_max_speed_bounds(const ifx_Avian_Calc_t* handle, double wavelength, double sampling_time, double* lower_bound, double* upper_bound)
{
    if (!handle->speed_measurement)
    {
        *upper_bound = 0;
        *lower_bound = 0;
        return;
    }

    /* max_speed <= wavelength/4 / (chirp_delay + sampling_time)
     *
     * The wavelength depends on the bandwidth because for a bandwidth smaller
     * than 500MHz we use the ISM band with a slightly different center
     * frequency. This should have almost no effect on the computations here.
     *
     * At this time we don't know the chirp_delay yet, so we assume the worst.
     * The chirp_delay is largest in active mode, so we use this for our bound.
     * This means that our bound is a bit too strict for power modes other than
     * active.
     *
     * sampling_time = num_samples_per_chirp / sample_rate. For most cases,
     * sampling_time >> chirp_delay.
     *
     * In addition, make sure that lower_bound >= MAX_SPEED_LOWER_LIMIT and
     * upper_bound <= MAX_SPEED_UPPER_LIMIT.
     */

    *upper_bound = MIN(MAX_SPEED_UPPER_LIMIT, wavelength / 4 / (handle->total_chirp_delay_active + sampling_time));
    *lower_bound = MAX(MAX_SPEED_LOWER_LIMIT, *upper_bound / 1000);
}

//----------------------------------------------------------------------------

void ifx_avian_calc_speed_resolution_bounds(const ifx_Avian_Calc_t* handle, double max_speed, double* lower_bound, double* upper_bound)
{
    if (!handle->speed_measurement)
    {
        *upper_bound = 0;
        *lower_bound = 0;
        return;
    }

    /* speed_resolution = 2*max_speed / num_chirps_per_frame
     *
     * The lower (upper) bound is given by setting MIN_NUM_CHIRPS_PER_FRAME
     * (MAX_NUM_CHIRPS_PER_FRAME) for num_chirps_per_frame in the equation
     * above.
     */

    *upper_bound = 2 * max_speed / MIN_NUM_CHIRPS_PER_FRAME;
    *lower_bound = 2 * max_speed / MAX_NUM_CHIRPS_PER_FRAME;
}

//----------------------------------------------------------------------------

double ifx_avian_calc_chirp_repetition_time(const ifx_Avian_Calc_t* handle, double max_speed, double wavelength)
{
    if (!handle->speed_measurement)
        return 0;

    return wavelength / 4 / max_speed;
}

//----------------------------------------------------------------------------

uint32_t ifx_avian_calc_num_chirps_per_frame(const ifx_Avian_Calc_t* handle, double max_speed, double speed_resolution)
{
    if (!handle->speed_measurement)
        return 1;

    const uint32_t ceil_ratio = (uint32_t)ceil(max_speed / speed_resolution);
    if (handle->power_of_twos)
        return 2 * ifx_math_round_up_power_of_2_uint32(ceil_ratio);
    else
        return 2 * ceil_ratio;
}

//----------------------------------------------------------------------------

double ifx_avian_calc_theoretical_speed_resolution(const ifx_Avian_Calc_t* handle, double max_speed, uint32_t num_chirps_per_frame)
{
    IGNORE(handle);
    return max_speed / num_chirps_per_frame * 2;
}

/* T_SED */
double ifx_avian_calc_shape_end_delay(const ifx_Avian_Calc_t* handle, bool mimo, double sampling_time, double pulse_repetition_time, ifx_avian_power_mode_t* shape_end_power_mode)
{
    if (!handle->speed_measurement)
    {
        if (shape_end_power_mode)
            *shape_end_power_mode = IFX_AVIAN_POWER_MODE_ACTIVE;
        return 0;
    }

    /* As we support MIMO as well as normal mode we introduce the notion of an
     * effective sampling time in addition to the actual sampling time.
     * In normal mode the effective sampling time is identical to the sampling
     * time.
     * In time-multiplex MIMO the shape group does not consist of one but two
     * chirps (first one with TX1 active, second with TX2 active). Hence the
     * effective sampling time in MIMO mode is two times the sampling time plus
     * the delay (total_chirp_delay_active) between the two chirps within the
     * shape group. The delay total_chirp_delay_active is the intermediate time
     * that the on-chip FSM and analog frontend need (PLL/VCO settling time...)
     * before the second physical chirp inside the sequence can be started.
     */
    const double effective_sampling_time = mimo
        ? 2 * sampling_time + handle->total_chirp_delay_active
        : sampling_time;

    /* The total time of a chirp (or in MIMO mode of a shape group) is the
     * pulse repetition time (PRT). The PRT is the sum of inevitable delays
     * that depend on on the shape end power mode, the effective sampling
     * time, and the configurable shape end delay:
     *      PRT = inevitable_delays + effective_sampling_time + shape_end_delay
     * The inevitable_delays depend on the power mode. The higher the power mode
     * the higher is inevitable_delays.
     * t0 is the pulse repetition time minus the effective sampling time:
     *      t0 = PRT - effective_sampling_time = inevitable_delays + shape_end_delay
     * We now determine the highest power mode which resulting delay is still
     * larger than t0. Once the power mode is found, shape end delay can be
     * calculated:
     *      shape_end_delay = t0 - inevitable_delays.
     */
    const double t0 = pulse_repetition_time - effective_sampling_time;

    if ((t0 - handle->total_chirp_delay_ds) > 0)
    {
        if (shape_end_power_mode)
            *shape_end_power_mode = IFX_AVIAN_POWER_MODE_DEEP_SLEEP_CONTINUE;
        return t0 - handle->total_chirp_delay_ds;
    }
    else if ((t0 - handle->total_chirp_delay_idle) > 0)
    {
        if (shape_end_power_mode)
            *shape_end_power_mode = IFX_AVIAN_POWER_MODE_IDLE;
        return t0 - handle->total_chirp_delay_idle;
    }
    else
    {
        if (shape_end_power_mode)
            *shape_end_power_mode = IFX_AVIAN_POWER_MODE_ACTIVE;
        return t0 - handle->total_chirp_delay_active;
    }
}

//----------------------------------------------------------------------------

double ifx_avian_calc_pulse_repetition_time_from_shape_end_delay(const ifx_Avian_Calc_t* handle, bool mimo, double shape_end_delay, double sampling_time, ifx_avian_power_mode_t shape_end_power_mode)
{
    double t0;
    if(mimo)
        t0 = shape_end_delay + 2 * sampling_time + handle->total_chirp_delay_active;
    else
        t0 = shape_end_delay + sampling_time;

    if (shape_end_power_mode == IFX_AVIAN_POWER_MODE_DEEP_SLEEP_CONTINUE)
        return t0 + handle->total_chirp_delay_ds;
    else if (shape_end_power_mode == IFX_AVIAN_POWER_MODE_IDLE)
        return t0 + handle->total_chirp_delay_idle;
    else if (shape_end_power_mode == IFX_AVIAN_POWER_MODE_ACTIVE)
        return t0 + handle->total_chirp_delay_active;
    else
        return 0;
}

//----------------------------------------------------------------------------

double ifx_avian_calc_range_bin_frequency(const ifx_Avian_Calc_t* handle, uint32_t sample_rate, uint32_t num_samples_per_chirp)
{
    IGNORE(handle);
    return (double)sample_rate / num_samples_per_chirp;
}

//----------------------------------------------------------------------------

double ifx_avian_calc_doppler_shift_vmax(const ifx_Avian_Calc_t* handle, double wavelength, double max_speed)
{
    IGNORE(handle);
    return 2 * max_speed / wavelength;
}

//----------------------------------------------------------------------------

double ifx_avian_calc_max_doppler_shift(const ifx_Avian_Calc_t* handle, double range_bin_frequency, double doppler_shift_vmax)
{
    IGNORE(handle);
    return doppler_shift_vmax / range_bin_frequency;
}

//----------------------------------------------------------------------------

void ifx_avian_calc_frame_repetition_time_bounds(const ifx_Avian_Calc_t* handle, double sampling_time, double pulse_repetition_time, uint32_t num_chirps_per_frame, double* lower_bound, double* upper_bound)
{
    const double min_frame_end_delay = 15e-9;
    const double min = ifx_avian_calc_frame_repetition_time_from_frame_end_delay(handle, min_frame_end_delay, sampling_time, pulse_repetition_time, num_chirps_per_frame, IFX_AVIAN_POWER_MODE_ACTIVE);
    *lower_bound = MAX(MIN_TIME_REPETITION_TIME_S, min);
    *upper_bound = MAX_TIME_REPETITION_TIME_S;
}

//----------------------------------------------------------------------------

double ifx_avian_calc_frame_repetition_time(const ifx_Avian_Calc_t* handle, double frame_rate)
{
    IGNORE(handle);
    return 1 / frame_rate;
}

//----------------------------------------------------------------------------

/* T_FED */
double ifx_avian_calc_frame_end_delay(const ifx_Avian_Calc_t* handle, double sampling_time, double pulse_repetition_time, uint32_t num_chirps_per_frame, double frame_time, ifx_avian_power_mode_t* frame_end_power_mode)
{
    /* The total time of a frame is the frame_time. The frame time is given as
     *      frame_time = PRT * (num_chirps_per_frame-1) + sampling_time + power_delay + frame_end_delay
     * where PRT denotes the pulse repetition time. power_delay is a delay that
     * depends on the chosen frame end power mode. The larger the frame end
     * power mode the larger is power_delay.
     * Similar like in ifx_avian_calc_shape_end_delay, we now determine the
     * most aggressive frame end power mode and then the frame end delay.
     */

    /* t0 = power_delay + frame_end_delay = frame_time - PRT * (num_chirps_per_frame-1) - sampling_time */
    const double t0 = frame_time - pulse_repetition_time * (num_chirps_per_frame - 1.0) - sampling_time;

    if ((t0 - handle->total_chirp_delay_ds) > 0)
    {
        if (frame_end_power_mode)
            *frame_end_power_mode = IFX_AVIAN_POWER_MODE_DEEP_SLEEP_CONTINUE;
        return t0 - handle->total_chirp_delay_ds;
    }
    else if ((t0 - handle->total_chirp_delay_idle) > 0)
    {
        if (frame_end_power_mode)
            *frame_end_power_mode = IFX_AVIAN_POWER_MODE_IDLE;
        return t0 - handle->total_chirp_delay_idle;
    }
    else
    {
        if (frame_end_power_mode)
            *frame_end_power_mode = IFX_AVIAN_POWER_MODE_ACTIVE;
        return t0 - handle->total_chirp_delay_active;
    }
}

//----------------------------------------------------------------------------

double ifx_avian_calc_frame_repetition_time_from_frame_end_delay(const ifx_Avian_Calc_t* handle, double frame_end_delay, double sampling_time, double pulse_repetition_time, uint32_t num_chirps_per_frame, ifx_avian_power_mode_t frame_end_power_mode)
{
    if (frame_end_power_mode == IFX_AVIAN_POWER_MODE_DEEP_SLEEP_CONTINUE)
    {
        return frame_end_delay + pulse_repetition_time * (num_chirps_per_frame - 1.0) + sampling_time + handle->total_chirp_delay_ds;
    }
    else if (frame_end_power_mode == IFX_AVIAN_POWER_MODE_IDLE)
    {
        return frame_end_delay + pulse_repetition_time * (num_chirps_per_frame - 1.0) + sampling_time + handle->total_chirp_delay_idle;
    }
    else if (frame_end_power_mode == IFX_AVIAN_POWER_MODE_ACTIVE)
    {
        return frame_end_delay + pulse_repetition_time * (num_chirps_per_frame - 1.0) + sampling_time + handle->total_chirp_delay_active;
    }
    else
        return 0;
}

//----------------------------------------------------------------------------

double ifx_avian_calc_time_DS(const ifx_Avian_Calc_t* handle, uint32_t num_chirps_per_frame, ifx_avian_power_mode_t shape_end_power_mode, double shape_end_delay, ifx_avian_power_mode_t frame_end_power_mode, double frame_end_delay)
{
    double time_DS = 0;

    if (shape_end_power_mode == IFX_AVIAN_POWER_MODE_DEEP_SLEEP_CONTINUE)
        time_DS += (shape_end_delay - handle->pre_chirp_time_ds) * (num_chirps_per_frame - 1.0);

    if (frame_end_power_mode == IFX_AVIAN_POWER_MODE_DEEP_SLEEP_CONTINUE)
        time_DS += frame_end_delay - handle->pre_chirp_time_ds;

    return time_DS;
}

//----------------------------------------------------------------------------

double ifx_avian_calc_time_idle(const ifx_Avian_Calc_t* handle, uint32_t num_chirps_per_frame, ifx_avian_power_mode_t shape_end_power_mode, double shape_end_delay, ifx_avian_power_mode_t frame_end_power_mode, double frame_end_delay)
{
    double time_idle = 0;

    if (shape_end_power_mode == IFX_AVIAN_POWER_MODE_DEEP_SLEEP_CONTINUE)
        time_idle += handle->idle_time_after_ds * (num_chirps_per_frame - 1.0);
    else if (shape_end_power_mode == IFX_AVIAN_POWER_MODE_IDLE)
        time_idle += shape_end_delay * (num_chirps_per_frame - 1.0);

    if (frame_end_power_mode == IFX_AVIAN_POWER_MODE_DEEP_SLEEP_CONTINUE)
        time_idle += handle->idle_time_after_ds;
    else if (frame_end_power_mode == IFX_AVIAN_POWER_MODE_IDLE)
        time_idle += frame_end_delay;

    return time_idle;
}

//----------------------------------------------------------------------------

double ifx_avian_calc_time_interchirp(const ifx_Avian_Calc_t* handle, uint32_t num_chirps_per_frame, ifx_avian_power_mode_t shape_end_power_mode, double shape_end_delay, ifx_avian_power_mode_t frame_end_power_mode, double frame_end_delay)
{
    double time_interchirp = 0;
    if (shape_end_power_mode == IFX_AVIAN_POWER_MODE_DEEP_SLEEP_CONTINUE || shape_end_power_mode == IFX_AVIAN_POWER_MODE_IDLE)
    {
        time_interchirp += handle->total_chirp_delay_idle * (num_chirps_per_frame - 1.0);
        if (shape_end_power_mode == IFX_AVIAN_POWER_MODE_DEEP_SLEEP_CONTINUE)
            time_interchirp += handle->total_chirp_delay_idle * (num_chirps_per_frame - 1.0);
    }
    else
        time_interchirp += (handle->total_chirp_delay_active + shape_end_delay) * (num_chirps_per_frame - 1.0);

    if (frame_end_power_mode == IFX_AVIAN_POWER_MODE_DEEP_SLEEP_CONTINUE || frame_end_power_mode == IFX_AVIAN_POWER_MODE_IDLE)
    {
        time_interchirp += handle->total_chirp_delay_idle;
        if (frame_end_power_mode == IFX_AVIAN_POWER_MODE_DEEP_SLEEP_CONTINUE)
            time_interchirp += handle->total_chirp_delay_idle;
    }
    else
        time_interchirp += handle->total_chirp_delay_active + frame_end_delay;

    return time_interchirp;
}

//----------------------------------------------------------------------------

double ifx_avian_calc_time_sampling(const ifx_Avian_Calc_t* handle, double sampling_time, uint32_t num_chirps_per_frame)
{
    IGNORE(handle);
    return num_chirps_per_frame * sampling_time;
}

//----------------------------------------------------------------------------

/*
double ifx_avian_calc_frame_time_check(double frame_time, double time_DS, double time_idle, double time_interchirp, double time_sampling)
{
    return frame_time - (time_DS + time_idle + time_interchirp + time_sampling);
}
*/

//----------------------------------------------------------------------------

double ifx_avian_calc_duty_cycle_sampling(const ifx_Avian_Calc_t* handle, double frame_time, double time_sampling)
{
    IGNORE(handle);
    return time_sampling / frame_time;
}

//----------------------------------------------------------------------------

double ifx_avian_calc_duty_cycle_active(const ifx_Avian_Calc_t* handle, double frame_time, double time_interchirp, double time_sampling)
{
    IGNORE(handle);
    return (time_interchirp + time_sampling) / frame_time;
}

//----------------------------------------------------------------------------

double ifx_avian_calc_power_DS(const ifx_Avian_Calc_t* handle)
{
    IGNORE(handle);
    return 0.00027;
}

//----------------------------------------------------------------------------

double ifx_avian_calc_power_idle(const ifx_Avian_Calc_t* handle)
{
    IGNORE(handle);
    return 0.00468;
}

//----------------------------------------------------------------------------

double ifx_avian_calc_power_interchirp(const ifx_Avian_Calc_t* handle, ifx_avian_angle_measurement_t angle_measurement)
{
    IGNORE(handle);
    // None 1, Azimuth, Elevation 2, AZ& EL 3
    switch (angle_measurement)
    {
    case IFX_AVIAN_ANGLE_MEASUREMENT_AZIMUTH:
    case IFX_AVIAN_ANGLE_MEASUREMENT_ELEVATION:
        return 0.235 + 2 * 0.025;
    case IFX_AVIAN_ANGLE_MEASUREMENT_AZIMUTH_ELEVATION:
        return 0.235 + 3 * 0.025;
    default: // ifx_avian_calc_ANGLE_MEASUREMENT_NONE
        return 0.235 + 1 * 0.025;
    }
}

//----------------------------------------------------------------------------

double ifx_avian_calc_power_sampling(const ifx_Avian_Calc_t* handle, ifx_avian_angle_measurement_t angle_measurement, uint8_t tx_power_setting)
{
    IGNORE(handle);
    // None 1, Azimuth, Elevation 2, AZ& EL 3
    switch (angle_measurement)
    {
    case IFX_AVIAN_ANGLE_MEASUREMENT_AZIMUTH:
    case IFX_AVIAN_ANGLE_MEASUREMENT_ELEVATION:
        return 0.245 + 0.027 * 2 + 0.001 * tx_power_setting;
    case IFX_AVIAN_ANGLE_MEASUREMENT_AZIMUTH_ELEVATION:
        return 0.245 + 0.027 * 3 + 0.001 * tx_power_setting;
    default: // ifx_avian_calc_ANGLE_MEASUREMENT_NONE
        return 0.245 + 0.027 * 1 + 0.001 * tx_power_setting;
    }
}

//----------------------------------------------------------------------------

double ifx_avian_calc_average_power(const ifx_Avian_Calc_t* handle, uint32_t num_chirps_per_frame, ifx_avian_power_mode_t shape_end_power_mode, double shape_end_delay, ifx_avian_power_mode_t frame_end_power_mode, double frame_end_delay, ifx_avian_angle_measurement_t angle_measurement, uint8_t tx_power, double frame_time, double sampling_time)
{
    const double timeSampling = ifx_avian_calc_time_sampling(handle, sampling_time, num_chirps_per_frame);
    const double timeIdle = ifx_avian_calc_time_idle(handle, num_chirps_per_frame, shape_end_power_mode, shape_end_delay, frame_end_power_mode, frame_end_delay);
    const double timeDS = ifx_avian_calc_time_DS(handle, num_chirps_per_frame, shape_end_power_mode, shape_end_delay, frame_end_power_mode, frame_end_delay);
    const double timeInterchirp = ifx_avian_calc_time_interchirp(handle, num_chirps_per_frame, shape_end_power_mode, shape_end_delay, frame_end_power_mode, frame_end_delay);

    const double powerDS = ifx_avian_calc_power_DS(handle);
    const double powerIdle = ifx_avian_calc_power_idle(handle);
    const double powerInterchirp = ifx_avian_calc_power_interchirp(handle, angle_measurement);
    const double powerSampling = ifx_avian_calc_power_sampling(handle, angle_measurement, tx_power);

    const double sum = timeDS * powerDS + timeIdle * powerIdle + timeInterchirp * powerInterchirp + timeSampling * powerSampling;
    return sum / frame_time;
}

//----------------------------------------------------------------------------

uint8_t ifx_avian_calc_number_of_antennas(const ifx_Avian_Calc_t* handle, ifx_avian_angle_measurement_t angle_measurement)
{
    IGNORE(handle);

    switch (angle_measurement)
    {
    case IFX_AVIAN_ANGLE_MEASUREMENT_AZIMUTH:
    case IFX_AVIAN_ANGLE_MEASUREMENT_ELEVATION:
        return 2;
    case IFX_AVIAN_ANGLE_MEASUREMENT_AZIMUTH_ELEVATION:
         return 3;
    default: // IFX_AVIAN_ANGLE_MEASUREMENT_NONE
        return 1;
    }
}
