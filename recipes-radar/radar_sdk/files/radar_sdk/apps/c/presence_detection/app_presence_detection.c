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
 * @file    app_presence_detection.c
 *
 * @brief   Presence detection application main program source file.
 *
 * This example illustrates how to fetch time-domain data from an Avian radar sensor
 * and process it utilizing a presence sensing algorithm of the Radar SDK.
 *
 */


/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
*/

#include <math.h>
#include <string.h>

#include "ifxAvian/Avian.h"
#include "ifxRadarSegmentation/RadarSegmentation.h"

#include "app_common.h"
#include "json.h"

/*
==============================================================================
   2. LOCAL DEFINITIONS
==============================================================================
*/
#define RX_ANTENNA_ID   0   // This implementation uses only 1 Rx Antenna

/*
==============================================================================
   3. LOCAL TYPES
==============================================================================
*/
typedef struct {
    ifx_PresenceSensing_t* presencesensing;
} presence_t;

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

/*
==============================================================================
   6. LOCAL FUNCTIONS
==============================================================================
*/

/**
 * @brief Callback function to print the information about presence state
 *
 * @param new_state     new presence sensing state
 * @param context       context of the application
 */
static void state_change_callback(ifx_PresenceSensing_State_t new_state, void* context)
{
    (void)context;
    switch(new_state){
        case IFX_PRESENCE_SENSING_PRESENT:
            app_print(", \"change\": \"present\"");
            break;
        case IFX_PRESENCE_SENSING_ABSENT:
            app_print(", \"change\": \"absent\"");
            break;
        default:
            break;
    }
}

//----------------------------------------------------------------------------

/**
 * @brief Application specific initialization function
 *
 * Initializes the presence sensing context.
 *
 * @param ctx           context of the application
 */
ifx_Error_t presence_init(presence_t* ctx)
{
    ctx->presencesensing = NULL;

    return IFX_OK;
}

/**
 * @brief Application specific configuration function
 *
 * Configures the presence sensing with specified in json or default configuration.
 * Defines a state change callback and creates an instance of presence sensing algorithm.
 *
 * @param ctx           context of the application
 * @param device        connected device handle
 * @param json          json handle with configuration
 * @param dev_config    device configuration handle
 */
ifx_Error_t presence_config(presence_t* ctx, ifx_Avian_Device_t* device, ifx_json_t* json, ifx_Avian_Config_t* dev_config)
{
    ifx_PresenceSensing_Config_t ps_config = { 0 };

    if (ifx_json_has_presence_sensing(json))
    {
        bool ret = ifx_json_get_presence_sensing(json, dev_config, &ps_config);
        if (!ret)
        {
            fprintf(stderr, "Error parsing presence sensing configuration: %s\n", ifx_json_get_error(json));
            return IFX_ERROR_APP;
        }
    }
    else
    {
        ifx_Radar_Sensor_t sensor = device
            ? ifx_avian_get_sensor_type(device) // determine radar sensor
            : IFX_AVIAN_BGT60TR13C; // if we opened a file assume a TR13C

        ifx_presence_sensing_get_config_defaults(sensor, dev_config, &ps_config);
    }

    ps_config.state_change_cb = state_change_callback;
    
    if(device)
    {
        //We use always last antenna as first one on some devices have leakage from tx (BGT60TR13C)
        const ifx_Radar_Sensor_Info_t* sensor_info = ifx_avian_get_sensor_information(device);
        dev_config->rx_mask = 1<<(sensor_info->num_rx_antennas-1u);
    }

    ctx->presencesensing = ifx_presence_sensing_create(&ps_config);
  
    return ifx_error_get();
}

/**
 * @brief Application specific cleanup function
 *
 * Destroys the presence sensing instances and cleans up allocated memory.
 *
 * @param ctx           context of the application
 */
ifx_Error_t presence_cleanup(presence_t* ctx)
{
    ifx_presence_sensing_destroy(ctx->presencesensing);

    return ifx_error_get();
}

/**
 * @brief Application specific processing function
 *
 * Runs a presence sensing algorithm on fetched data and informs about the results.
 *
 * @param ctx           context of the application
 * @param frame         collected frame
 */
ifx_Error_t presence_process(presence_t* presence_context, ifx_Cube_R_t* frame)
{
    ifx_PresenceSensing_Result_t ps_result = { 0 };
    ifx_Matrix_R_t antenna_data;
    ifx_cube_get_row_r(frame, RX_ANTENNA_ID, &antenna_data);
    ifx_presence_sensing_run(presence_context->presencesensing, &antenna_data, &ps_result);
    if (ifx_error_get() != IFX_OK)
        return ifx_error_get();

    if (ps_result.cur_presence_state == IFX_PRESENCE_SENSING_PRESENT)
        app_print(", \"is_present\": true");
    else //if(cur_state == ABSENCE)
        app_print(", \"is_present\": false");

    if (!isnan(ps_result.target_distance_m) && !isnan(ps_result.target_speed_m_s))
    {
        app_verbose(", \"target_distance_m\": %g, \"target_speed_m_s\": %g, \"target_signal_strength\": %g",
            ps_result.target_distance_m, ps_result.target_speed_m_s, ps_result.target_signal_strength);
    }

    return ifx_error_get();
}

/*
==============================================================================
   7. MAIN METHOD
==============================================================================
 */

int main(int argc, char* argv[])
{
    app_t s_presence = { 0 };
    presence_t presence_context = { 0 };
    int exitcode;

    static const char* app_description = "Presence detection";
    static const char* app_epilog = NULL;

    s_presence.app_description = app_description;
    s_presence.app_epilog = app_epilog;

    s_presence.app_init = (void*)&presence_init;
    s_presence.app_config = (void*)&presence_config;
    s_presence.app_process = (void*)&presence_process;
    s_presence.app_cleanup = (void*)&presence_cleanup;

    s_presence.default_metrics = NULL;

    exitcode = app_start(argc, argv, &s_presence, &presence_context);

    return exitcode;
}
