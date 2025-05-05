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
 * @file PresenceSensing.c
 *
 * @brief This file implements the API for presence sensing use case.
 *
 */

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
*/

#include <math.h> // for NAN
#include <string.h> // for memset

#include "ifxAlgo/MTI.h"

#include "ifxBase/Error.h"
#include "ifxBase/Matrix.h"
#include "ifxBase/Mem.h"
#include "ifxBase/Vector.h"

#include "ifxRadar/PeakSearch.h"
#include "ifxRadar/PresenceSensing.h"
#include "ifxRadar/RangeSpectrum.h"
#include "ifxRadar/SpectrumAxis.h"


/*
==============================================================================
   2. LOCAL DEFINITIONS
==============================================================================
*/

#define MAX_NUM_OF_TARGETS         5

/* default values for presence sensing configuration */
#define RANGE_FFT_SIZE_DEFAULT 128
#define RANGE_FFT_WINDOW_TYPE_DEFAULT IFX_WINDOW_BLACKMANNHARRIS
#define RANGE_FFT_WINDOW_ALPHA_DEFAULT 0

#define DOPPLER_FFT_SIZE_DEFAULT 128
#define DOPPLER_FFT_WINDOW_TYPE_DEFAULT IFX_WINDOW_CHEBYSHEV
#define DOPPLER_FFT_WINDOW_ALPHA_DEFAULT 60

#define MTI_WEIGHT_DEFAULT 1
#define RANGE_SPECTRUM_MODE_DEFAULT IFX_RS_MODE_MAX_BIN
#define MIN_DETECTION_RANGE_M_DEFAULT ((ifx_Float_t)0.2)
#define MAX_DETECTION_RANGE_M_DEFAULT 2
#define RANGE_HYSTERESIS_PERCENT_DEFAULT 10

#define PRESENCE_CONFIRM_COUNT_DEFAULT 5
#define ABSENCE_CONFIRM_COUNT_DEFAULT 4

#define THRESHOLD_FACTOR_PRESENCE_PEAK_DEFAULT ((ifx_Float_t)3)
#define THRESHOLD_FACTOR_ABSENCE_PEAK_DEFAULT  ((ifx_Float_t)4)
#define THRESHOLD_ABSENCE_FINE_PEAK_DEFAULT    ((ifx_Float_t)1.5)

/*
==============================================================================
   3. LOCAL TYPES
==============================================================================
*/

typedef struct {
    ifx_Vector_R_t* fft_spectrum_result;
    ifx_Matrix_C_t* frame_fft_half_result;
} ifx_RangeFFT_t;

typedef struct {
    ifx_Float_t    doppler_threshold;
    ifx_Vector_C_t* fft_data;
    ifx_Vector_C_t* prepro_result;
    ifx_Vector_C_t* chirp_fft_result;
    ifx_Vector_R_t* abs_result;
    ifx_Matrix_R_t* range_doppler_map;
} ifx_DopplerFFT_t;

struct ifx_PresenceSensing_s
{
    ifx_PresenceSensing_State_t  state;

    ifx_PresenceSensing_StatusCallback_t state_status_cb;
    ifx_PresenceSensing_ChangeCallback_t state_change_cb;
    void* context_callback;

    uint32_t cur_peak_count;
    uint32_t doppler_obj_count;
    uint32_t definitive_hello_count;
    uint32_t hello_counter;
    uint32_t definitive_bye_count;
    uint32_t bye_counter;
    uint32_t global_counter;

    ifx_RS_t* range_spectrum_handle;
    ifx_RangeFFT_t   range_spectrum_data;
    ifx_DopplerFFT_t doppler_data;
    ifx_Vector_R_t* mti_result;

    ifx_Vector_R_t* doppler_fft_window;
    ifx_FFT_t*      doppler_fft_handle;

    ifx_Peak_Search_t* presence_peak_handle;
    ifx_Peak_Search_t* absence_peak_handle;
    ifx_Peak_Search_t* absence_fine_peak_handle;
    ifx_Peak_Search_t* peak_search_handle;
    uint32_t                 num_targets;

    ifx_MTI_t* mti_handle;

    ifx_Float_t range_resolution_m;
    ifx_Float_t speed_resolution_m_s;
};

/*
==============================================================================
   4. LOCAL DATA
==============================================================================
*/

/*
==============================================================================
   5. LOCAL FUNCTION PROTOTYPES
==============================================================================
*/

static void state_machine(ifx_PresenceSensing_t* handle);

/*
==============================================================================
   6. LOCAL FUNCTIONS
==============================================================================
*/

void state_machine(ifx_PresenceSensing_t* handle)
{
    if (handle->state == IFX_PRESENCE_SENSING_PRESENT)
    {
        if (handle->cur_peak_count == 0)
        {
            ++handle->bye_counter;
        }

        if ( handle->bye_counter == handle->definitive_bye_count &&
             handle->global_counter == handle->definitive_bye_count )
        {
            handle->state = IFX_PRESENCE_SENSING_ABSENT;

            if (handle->state_change_cb != NULL)
            {
                handle->state_change_cb(handle->state, handle->context_callback);
            }

            handle->peak_search_handle = handle->absence_peak_handle;
        }
    }
    else //if(handle->state == ABSENCE)
    {
        if ( handle->cur_peak_count > 0 || handle->doppler_obj_count > 0)
        {
            ++handle->hello_counter;
        }
        else
        {
            handle->hello_counter = 0;
        }

        if (handle->hello_counter == handle->definitive_hello_count)
        {
            handle->state = IFX_PRESENCE_SENSING_PRESENT;

            if (handle->state_change_cb != NULL)
            {
                handle->state_change_cb(handle->state, handle->context_callback);
            }

            handle->peak_search_handle = handle->presence_peak_handle;
        }
    }

    if (handle->global_counter == handle->definitive_bye_count)
    {
        handle->bye_counter = 0;
        handle->global_counter = 0;
    }
}

/*
==============================================================================
   7. EXPORTED FUNCTIONS
==============================================================================
*/

void ifx_presence_sensing_get_config_defaults(ifx_Radar_Sensor_t sensor_type, ifx_Avian_Config_t* device_config, ifx_PresenceSensing_Config_t* config)
{
    IFX_ERR_BRK_NULL(device_config);
    IFX_ERR_BRK_NULL(config);

    ifx_Avian_Device_t* handle = ifx_avian_create_dummy(sensor_type);
    if (!handle)
        return;


    const ifx_Radar_Sensor_Info_t* sensor_info = ifx_avian_get_sensor_information(handle);
    const uint8_t num_rx = sensor_info->num_rx_antennas;
    ifx_avian_destroy(handle);

    /* default device configuration for presence sensing */
    device_config->sample_rate_Hz = 1000000;         // 1 MHz
    device_config->rx_mask = (num_rx >= 3) ? 4 : 1;  // Use RX1-3 when device has at least 3 antennas, RX1 otherwise
    device_config->tx_mask = 1;
    device_config->tx_power_level = TX_POWER_LEVEL_UPPER;
    device_config->if_gain_dB = 33;                  // 33 dB
    device_config->start_frequency_Hz = 60020000000; // 60.02 GHz
    device_config->end_frequency_Hz = 61020000000; // 61.02 GHz
    device_config->num_samples_per_chirp = 128;
    device_config->num_chirps_per_frame = 64;
    device_config->chirp_repetition_time_s = 0.00050564f; // 505.64 µs
    device_config->frame_repetition_time_s = 0.2f;    // frame rate of 5 Hz
    device_config->hp_cutoff_Hz = 80000;
    device_config->aaf_cutoff_Hz = 500000;
    device_config->mimo_mode = IFX_MIMO_OFF;
 
    memset(config, 0, sizeof(ifx_PresenceSensing_Config_t));

    /* copy device config */
    config->device_config = *device_config;

    /* range FFT */
    config->range_fft_size = RANGE_FFT_SIZE_DEFAULT;
    config->range_fft_window_type = IFX_WINDOW_BLACKMANHARRIS;
    config->range_fft_window_alpha = RANGE_FFT_WINDOW_ALPHA_DEFAULT;

    /* Doppler FFT */
    config->doppler_fft_size = DOPPLER_FFT_SIZE_DEFAULT;
    config->doppler_fft_window_type = IFX_WINDOW_CHEBYSHEV;
    config->doppler_fft_window_alpha = DOPPLER_FFT_WINDOW_ALPHA_DEFAULT;

    config->mti_weight = MTI_WEIGHT_DEFAULT;
    config->range_spectrum_mode = RANGE_SPECTRUM_MODE_DEFAULT;
    config->min_detection_range_m = MIN_DETECTION_RANGE_M_DEFAULT;
    config->max_detection_range_m = MAX_DETECTION_RANGE_M_DEFAULT;
    config->range_hysteresis_percentage = RANGE_HYSTERESIS_PERCENT_DEFAULT;

    /* counts */
    config->presence_confirm_count = PRESENCE_CONFIRM_COUNT_DEFAULT;
    config->absence_confirm_count = ABSENCE_CONFIRM_COUNT_DEFAULT;

    /* thresholds */
    config->threshold_factor_presence_peak = THRESHOLD_FACTOR_PRESENCE_PEAK_DEFAULT;
    config->threshold_factor_absence_peak = THRESHOLD_FACTOR_ABSENCE_PEAK_DEFAULT;
    config->threshold_factor_absence_fine_peak = THRESHOLD_ABSENCE_FINE_PEAK_DEFAULT;

    /* callbacks */
    config->state_status_cb = NULL;
    config->state_change_cb = NULL;
    config->context_callback = NULL;
}

ifx_PresenceSensing_t* ifx_presence_sensing_create(const ifx_PresenceSensing_Config_t* config)
{
    IFX_ERR_BRN_NULL(config);

    /************************ Verify parameter bounds **************************/
    /* range_hysteresis_percentage */
    IFX_ERR_BRN_COND(config->range_hysteresis_percentage < 0.f, IFX_ERROR_ARGUMENT_OUT_OF_BOUNDS);
    IFX_ERR_BRN_COND(config->range_hysteresis_percentage > 100.f, IFX_ERROR_ARGUMENT_OUT_OF_BOUNDS);

    /* min_detection_range_m */
    IFX_ERR_BRN_COND(config->min_detection_range_m < 0.f, IFX_ERROR_ARGUMENT_OUT_OF_BOUNDS);
    IFX_ERR_BRN_COND((config->min_detection_range_m > (config->max_detection_range_m*(1.0f-(config->range_hysteresis_percentage)/100.0f))),
        IFX_ERROR_ARGUMENT_OUT_OF_BOUNDS);

    ifx_PresenceSensing_t* h = ifx_mem_calloc(1, sizeof(struct ifx_PresenceSensing_s));
    IFX_ERR_BRN_MEMALLOC(h);

    const ifx_Avian_Config_t* device_config = &config->device_config;

    h->state = IFX_PRESENCE_SENSING_ABSENT;
    h->state_status_cb = config->state_status_cb;
    h->state_change_cb = config->state_change_cb;
    h->context_callback = config->context_callback;

    h->definitive_hello_count = config->presence_confirm_count;
    h->hello_counter = 0;
    h->definitive_bye_count = config->absence_confirm_count;
    h->bye_counter = 0;
    h->global_counter = 0;

    h->doppler_obj_count = 0;

    /***************************** Range Windowing **************************/

    ifx_RS_Config_t range_spectrum_config =
    {
            .fft_config =
            {
                    .fft_type = IFX_FFT_TYPE_R2C,
                    .fft_size = config->range_fft_size,
                    .mean_removal_enabled = 1,
                    .window_config =
                     {
                          .type = config->range_fft_window_type,
                          .size = device_config->num_samples_per_chirp,
                          .at_dB = config->range_fft_window_alpha
                     },
                    .is_normalized_window = 1
            },
            .num_of_chirps_per_frame = device_config->num_chirps_per_frame,
            .output_scale_type = IFX_SCALE_TYPE_LINEAR,
            .spect_threshold = 0
    };

    IFX_ERR_HANDLE_N(h->range_spectrum_handle = ifx_rs_create(&range_spectrum_config),
                 ifx_presence_sensing_destroy(h));

    ifx_rs_set_mode(h->range_spectrum_handle, config->range_spectrum_mode);

    h->range_spectrum_data.fft_spectrum_result = ifx_vec_create_r(config->range_fft_size / 2);

    h->range_spectrum_data.frame_fft_half_result =
        ifx_mat_create_c(device_config->num_chirps_per_frame, config->range_fft_size / 2);

    /***************************** Doppler Windowing **************************/

    ifx_Window_Config_t doppler_fft_window_config;
    doppler_fft_window_config.type = config->doppler_fft_window_type;
    doppler_fft_window_config.size = device_config->num_chirps_per_frame;
    doppler_fft_window_config.at_dB = config->doppler_fft_window_alpha;

    IFX_ERR_HANDLE_N(h->doppler_fft_window = ifx_vec_create_r(doppler_fft_window_config.size),
                 ifx_presence_sensing_destroy(h));

    IFX_ERR_HANDLE_N(ifx_window_init(&doppler_fft_window_config, h->doppler_fft_window),
              ifx_presence_sensing_destroy(h));

    // normalize such that the sum(doppler_fft_window) = 1
    const ifx_Float_t sum = ifx_vec_sum_r(h->doppler_fft_window);
    if(sum)
        ifx_vec_scale_r(h->doppler_fft_window, 1/sum, h->doppler_fft_window);

    /***************************** Doppler FFT ************************************/
    h->doppler_data.fft_data = ifx_vec_create_c(device_config->num_chirps_per_frame);
    h->doppler_fft_handle = ifx_fft_create(IFX_FFT_TYPE_C2C, config->doppler_fft_size);
    h->doppler_data.prepro_result = ifx_vec_create_c(device_config->num_chirps_per_frame);
    h->doppler_data.chirp_fft_result = ifx_vec_create_c(config->doppler_fft_size);
    h->doppler_data.abs_result = ifx_vec_create_r(config->doppler_fft_size);
        
    const ifx_Float_t center_rf_freq_Hz = ifx_devconf_get_center_frequency(device_config);
    ifx_Float_t chirptime_s = ifx_devconf_get_chirp_time(device_config);

    h->speed_resolution_m_s = ifx_spectrum_axis_calc_speed_per_bin(config->doppler_fft_size, center_rf_freq_Hz, chirptime_s);

    /********************************* Algorithm parameters **************************/

    h->doppler_data.doppler_threshold = 0.0002f;

    const ifx_Float_t bandwidth_Hz = ifx_devconf_get_bandwidth(device_config);
    ifx_Float_t dist_per_bin = ifx_spectrum_axis_calc_dist_per_bin(config->range_fft_size, device_config->num_samples_per_chirp, bandwidth_Hz);
    h->range_resolution_m = dist_per_bin;

    ifx_Float_t active_zone_m = config->max_detection_range_m;
    ifx_Float_t min_range_m = config->min_detection_range_m;

    ifx_Float_t hysteresis_m = active_zone_m * config->range_hysteresis_percentage / 100.0f;

    // peak search handle
    ifx_Peak_Search_Config_t presence_peak_search_config = {
        .value_per_bin = dist_per_bin,
        .search_zone_start = min_range_m,
        .search_zone_end = active_zone_m + hysteresis_m,
        .threshold_factor = config->threshold_factor_presence_peak,
        .threshold_offset = 0,
        .max_num_peaks = 1};

    IFX_ERR_HANDLE_N(h->presence_peak_handle = ifx_peak_search_create(&presence_peak_search_config),
                 ifx_presence_sensing_destroy(h));

    ifx_Peak_Search_Config_t absence_peak_search_config = {
        .value_per_bin = dist_per_bin,
        .search_zone_start = min_range_m,
        .search_zone_end = active_zone_m - hysteresis_m,
        .threshold_factor = config->threshold_factor_absence_peak,
        .threshold_offset = 0,
        .max_num_peaks = MAX_NUM_OF_TARGETS
    };

    IFX_ERR_HANDLE_N(h->absence_peak_handle = ifx_peak_search_create(&absence_peak_search_config),
                 ifx_presence_sensing_destroy(h));

    ifx_Peak_Search_Config_t absence_fine_peak_search_config = {
        .value_per_bin = dist_per_bin,
        .search_zone_start = min_range_m,
        .search_zone_end = active_zone_m,
        .threshold_factor = config->threshold_factor_absence_fine_peak,
        .threshold_offset = 0,
        .max_num_peaks = MAX_NUM_OF_TARGETS
    };

    IFX_ERR_HANDLE_N(h->absence_fine_peak_handle = ifx_peak_search_create(&absence_fine_peak_search_config),
                 ifx_presence_sensing_destroy(h));

    h->peak_search_handle = h->absence_peak_handle;

    h->mti_handle = ifx_mti_create(config->mti_weight, IFX_VEC_LEN(h->range_spectrum_data.fft_spectrum_result));

    h->mti_result = ifx_vec_create_r(IFX_VEC_LEN(h->range_spectrum_data.fft_spectrum_result));
    
    return h;
}

//----------------------------------------------------------------------------

void ifx_presence_sensing_destroy(ifx_PresenceSensing_t* handle)
{
    if (handle == NULL)
        return;

    ifx_vec_destroy_r(handle->range_spectrum_data.fft_spectrum_result);

    ifx_mat_destroy_c(handle->range_spectrum_data.frame_fft_half_result);

    // Doppler data
    ifx_vec_destroy_r(handle->doppler_fft_window);
    ifx_vec_destroy_c(handle->doppler_data.fft_data);
    ifx_fft_destroy(handle->doppler_fft_handle);
    ifx_vec_destroy_c(handle->doppler_data.prepro_result);
    ifx_vec_destroy_c(handle->doppler_data.chirp_fft_result);
    ifx_vec_destroy_r(handle->doppler_data.abs_result);
    ifx_mat_destroy_r(handle->doppler_data.range_doppler_map);

    // peak search handle
    ifx_peak_search_destroy(handle->presence_peak_handle);

    ifx_peak_search_destroy(handle->absence_peak_handle);

    ifx_peak_search_destroy(handle->absence_fine_peak_handle);

    handle->peak_search_handle = NULL;

    ifx_mti_destroy(handle->mti_handle);

    ifx_vec_destroy_r(handle->mti_result);

    ifx_rs_destroy(handle->range_spectrum_handle);

    ifx_mem_free(handle);
}

//----------------------------------------------------------------------------

void ifx_presence_sensing_run(ifx_PresenceSensing_t* handle, const ifx_Matrix_R_t* frame_data, ifx_PresenceSensing_Result_t* result)
{
    IFX_ERR_BRK_NULL(frame_data);
    IFX_ERR_BRK_NULL(handle);
    IFX_ERR_BRK_NULL(result);

    ++handle->global_counter;

    // reset state
    ifx_mat_clear_c(handle->range_spectrum_data.frame_fft_half_result);
    handle->doppler_obj_count = 0;

    /*************************************************************************************/
    /*********************************** Range Processing ********************************/
    /*************************************************************************************/

    ifx_rs_run_r(handle->range_spectrum_handle, frame_data,
                 handle->range_spectrum_data.fft_spectrum_result);

    ifx_rs_copy_fft_matrix(handle->range_spectrum_handle,
                           handle->range_spectrum_data.frame_fft_half_result);

    /**************************** Remove static objects (MTI filter) *********************/

    ifx_vec_copy_r(handle->range_spectrum_data.fft_spectrum_result, handle->mti_result);
    ifx_mti_run (handle->mti_handle, handle->mti_result, handle->mti_result);
    // peak search requires absolute values
    ifx_vec_abs_r(handle->mti_result, handle->mti_result);

    // presence peak search
    ifx_Peak_Search_Result_t cur_peak_search_result = {0};
    ifx_peak_search_run(handle->peak_search_handle,
                              handle->mti_result,
                              &cur_peak_search_result);
    handle->cur_peak_count = cur_peak_search_result.peak_count;

    // absence peak search
    ifx_Peak_Search_Result_t fine_peak_result = {0};
    ifx_peak_search_run(handle->absence_fine_peak_handle,
                        handle->mti_result,
                        &fine_peak_result);
    handle->num_targets = fine_peak_result.peak_count;

    /*************************************************************************************/
    /********************************** Doppler Processing *******************************/
    /*************************************************************************************/

    /* speed, distance, and strength of dominant peak */
    ifx_Float_t target_speed = NAN;
    ifx_Float_t target_distance = NAN;
    ifx_Float_t target_signal_strength = NAN;

    for (uint32_t i = 0; i < fine_peak_result.peak_count; ++i)
    {
        uint32_t pidx = fine_peak_result.index[i];

        // transpose peak range results into fft input matrix
        for (uint32_t midx = 0; midx < IFX_MAT_ROWS(handle->range_spectrum_data.frame_fft_half_result); ++midx)
        {
            ifx_Complex_t element = IFX_MAT_AT(handle->range_spectrum_data.frame_fft_half_result, midx, pidx);
            IFX_VEC_AT(handle->doppler_data.fft_data, midx) = element;
        }

        /******************************* Doppler Preprocessing Start ***********************/

        // 1. Mean removal
        ifx_Complex_t mean = ifx_vec_mean_c(handle->doppler_data.fft_data);

        ifx_vec_sub_cs(handle->doppler_data.fft_data, mean, handle->doppler_data.prepro_result);

        // 2. windowing (apply window function)
        ifx_vec_mul_cr(handle->doppler_data.prepro_result, handle->doppler_fft_window, handle->doppler_data.prepro_result);

        /******************************* Doppler Preprocessing End ***********************/

        // 3. doppler fft
        ifx_fft_run_c(handle->doppler_fft_handle, handle->doppler_data.prepro_result, handle->doppler_data.chirp_fft_result);

        ifx_vec_shift_c(handle->doppler_data.chirp_fft_result, IFX_VEC_LEN(handle->doppler_data.chirp_fft_result)/2);

        ifx_vec_abs_c(handle->doppler_data.chirp_fft_result, handle->doppler_data.abs_result);

        // copy range Doppler map vectors only if user requested i.e. allocated memory for result vector
        if(handle->doppler_data.range_doppler_map != NULL)
        {
            ifx_mat_set_row_vector_r(handle->doppler_data.range_doppler_map, i, handle->doppler_data.abs_result);
        }

        // count inward moving targets
        uint32_t max_idx;
        ifx_Float_t max_val = ifx_math_find_max(handle->doppler_data.abs_result, &max_idx);

        if (max_val > handle->doppler_data.doppler_threshold && max_idx < IFX_VEC_LEN(handle->doppler_data.abs_result) / 2)
            ++handle->doppler_obj_count;

        /* if this is the first peak (target_signal_strength is NAN) or the
         * current peak is stronger than the previously found peak, update
         * target_signal_strength, target_speed, target_distance.
         */
        if (isnan(target_signal_strength) || max_val > target_signal_strength)
        {
            const int32_t speed_idx = (int32_t)(IFX_VEC_LEN(handle->doppler_data.abs_result) / 2) - (int32_t)max_idx;

            target_signal_strength = max_val;
            target_speed = (ifx_Float_t)speed_idx * handle->speed_resolution_m_s;
            target_distance = (ifx_Float_t)pidx * handle->range_resolution_m;
        }
    }
    // state machine
    state_machine(handle);

    result->cur_presence_state = handle->state;
    result->range_doppler_map = handle->doppler_data.range_doppler_map;
    result->range_spectrogram = handle->range_spectrum_data.frame_fft_half_result;
    result->num_targets = handle->num_targets;
    result->target_distance_m = target_distance;
    result->target_speed_m_s = target_speed;
    result->target_signal_strength = target_signal_strength;

    if (handle->state_status_cb != NULL)
    {
        handle->state_status_cb(handle->state, handle->context_callback);
    }
}
