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

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
*/

#include "ifxAvian/DeviceControl.h"
#include "ifxAvian/internal/DeviceCalc.h"
#include "ifxAvian/internal/RadarDevice.hpp"
#include "ifxRadarDeviceCommon/internal/RadarDeviceCommon.hpp"
#include <platform/exception/EConnection.hpp>
#include <ifxBase/Exception.hpp>

#include "ifxAvian/Metrics.h"

/*
==============================================================================
   2. LOCAL DEFINITIONS
==============================================================================
*/

/*
==============================================================================
   3. LOCAL TYPES
==============================================================================
*/

/*
==============================================================================
   4. FUNCTION PROTOTYPES
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

/*
==============================================================================
   7. EXPORTED FUNCTIONS
==============================================================================
*/

//----------------------------------------------------------------------------

void ifx_avian_set_config(ifx_Avian_Device_t* handle, const ifx_Avian_Config_t* config)
{
    IFX_ERR_BRK_NULL(handle);
    IFX_ERR_BRK_NULL(config);

    auto set_config = [&handle, &config]() {
        handle->set_config(*config);
    };

    rdk::RadarDeviceCommon::exec_func(set_config);
}

//----------------------------------------------------------------------------

void ifx_avian_get_config(ifx_Avian_Device_t* handle, ifx_Avian_Config_t* config)
{
    IFX_ERR_BRK_NULL(handle);
    IFX_ERR_BRK_NULL(config);

    auto get_config = [&handle, &config]() {
        handle->get_config(*config);
    };

    rdk::RadarDeviceCommon::exec_func(get_config);
}

//----------------------------------------------------------------------------

void ifx_avian_get_config_defaults(ifx_Avian_Device_t* handle, ifx_Avian_Config_t* config)
{
    IFX_ERR_BRK_NULL(handle);
    IFX_ERR_BRK_NULL(config);

    ifx_Avian_Metrics_t default_metrics;
    ifx_avian_metrics_get_defaults(handle, &default_metrics);
    ifx_avian_metrics_to_config(handle, &default_metrics, config, true);
}

//----------------------------------------------------------------------------

void ifx_avian_config_get_default_limits(const ifx_Avian_Device_t* handle, ifx_Avian_Config_t* config_lower, ifx_Avian_Config_t* config_upper)
{
    IFX_ERR_BRK_NULL(handle);
    IFX_ERR_BRK_NULL(config_lower);
    IFX_ERR_BRK_NULL(config_upper);

    auto get_limits = [&handle, &config_lower, &config_upper]() {
        return handle->config_get_limits(*config_lower, *config_upper);
    };

    rdk::RadarDeviceCommon::exec_func(get_limits);
}

//----------------------------------------------------------------------------

ifx_Float_t ifx_avian_config_get_min_chirp_repetition_time(const ifx_Avian_Device_t* handle, const ifx_Avian_Config_t* config)
{
    IFX_ERR_BRV_NULL(handle, 0);
    IFX_ERR_BRV_NULL(config, 0);

    auto get_chirp_repetition_time = [&handle, config]() {
        return handle->get_min_chirp_repetition_time(*config);
    };

    return rdk::RadarDeviceCommon::exec_func<ifx_Float_t>(get_chirp_repetition_time, 0);
}

//----------------------------------------------------------------------------

ifx_Float_t ifx_avian_config_get_min_frame_repetition_time(const ifx_Avian_Device_t* handle, const ifx_Avian_Config_t* config)
{
    IFX_ERR_BRV_NULL(handle, 0);
    IFX_ERR_BRV_NULL(config, 0);
    
    auto get_min_frame_repetition_time = [&handle, config]() {
        return handle->get_min_frame_repetition_time(*config);
    };

    return rdk::RadarDeviceCommon::exec_func<ifx_Float_t>(get_min_frame_repetition_time, 0);
}

//----------------------------------------------------------------------------

void ifx_avian_get_temperature(ifx_Avian_Device_t* handle, ifx_Float_t* temperature_celsius)
{
    IFX_ERR_BRK_NULL(handle);
    IFX_ERR_BRK_NULL(temperature_celsius);

    auto get_temperature = [&handle]() {
        return handle->get_temperature();
    };

    *temperature_celsius = rdk::RadarDeviceCommon::exec_func<ifx_Float_t>(get_temperature, NAN);
}

//----------------------------------------------------------------------------

void ifx_avian_configure_adc(ifx_Avian_Device_t* handle, const ifx_Avian_ADC_Config_t* config)
{
    IFX_ERR_BRK_NULL(handle);
    IFX_ERR_BRK_NULL(config);

    auto configure_adc = [&handle, &config]() {
        handle->configure_adc(*config);
    };

    rdk::RadarDeviceCommon::exec_func(configure_adc);
}

//----------------------------------------------------------------------------

void ifx_avian_destroy(ifx_Avian_Device_t* handle)
{
    auto destroy_handle = [&handle]() {
        delete handle;
    };

    rdk::RadarDeviceCommon::exec_func(destroy_handle);
}

//----------------------------------------------------------------------------

void ifx_avian_start_acquisition(ifx_Avian_Device_t* handle)
{
    IFX_ERR_BRK_NULL(handle);

    auto start_acquisition = [&handle]() {
        handle->start_acquisition();
    };

    rdk::RadarDeviceCommon::exec_func(start_acquisition);
}

//----------------------------------------------------------------------------

void ifx_avian_stop_acquisition(ifx_Avian_Device_t* handle)
{
    IFX_ERR_BRK_NULL(handle);

    auto stop_acquisition = [&handle]() {
        handle->stop_acquisition();
    };

    rdk::RadarDeviceCommon::exec_func(stop_acquisition);
}

//----------------------------------------------------------------------------

ifx_Cube_R_t* ifx_avian_get_next_frame(ifx_Avian_Device_t* handle, ifx_Cube_R_t* frame)
{
    IFX_ERR_BRN_NULL(handle);

    auto get_next_frame_timeout = [&handle, &frame]() {
        return ifx_avian_get_next_frame_timeout(handle, frame, 10000);
    };

    return rdk::RadarDeviceCommon::exec_func<ifx_Cube_R_t*>(get_next_frame_timeout, nullptr);
}

//----------------------------------------------------------------------------

ifx_Cube_R_t* ifx_avian_get_next_frame_timeout(ifx_Avian_Device_t* handle, ifx_Cube_R_t* frame, uint16_t timeout_ms)
{
    IFX_ERR_BRN_NULL(handle);

    auto get_next_frame = [&handle, &frame, &timeout_ms]() {
        return handle->get_next_frame(frame, timeout_ms);
    };

    return rdk::RadarDeviceCommon::exec_func<ifx_Cube_R_t*>(get_next_frame, nullptr);
}

//----------------------------------------------------------------------------

uint8_t ifx_avian_get_num_rx_antennas(ifx_Avian_Device_t* handle)
{
    IFX_ERR_BRV_NULL(handle, 0);
    return handle->get_sensor_info()->num_rx_antennas;
}

//----------------------------------------------------------------------------

uint8_t ifx_avian_get_num_tx_antennas(ifx_Avian_Device_t* handle)
{
    IFX_ERR_BRV_NULL(handle, 0);
    return handle->get_sensor_info()->num_tx_antennas;
}

//----------------------------------------------------------------------------

const char* ifx_avian_get_board_uuid(const ifx_Avian_Device_t* handle)
{
    IFX_ERR_BRV_NULL(handle, nullptr);
    return handle->get_board_uuid();
}

//----------------------------------------------------------------------------

const ifx_Radar_Sensor_Info_t* ifx_avian_get_sensor_information(const ifx_Avian_Device_t* handle)
{
    IFX_ERR_BRV_NULL(handle, nullptr);
    return handle->get_sensor_info();
}

//----------------------------------------------------------------------------

const ifx_Firmware_Info_t* ifx_avian_get_firmware_information(const ifx_Avian_Device_t* handle)
{
    IFX_ERR_BRV_NULL(handle, nullptr);
    return handle->get_firmware_info();
}

//----------------------------------------------------------------------------

void ifx_avian_get_shield_information(const ifx_Avian_Device_t* handle, ifx_RF_Shield_Info_t* shield_info)
{
    IFX_ERR_BRK_NULL(handle);
    IFX_ERR_BRK_NULL(shield_info);
    auto *board = handle->get_strata_avian_board();
    ifx_Error_t error_code = rdk::RadarDeviceCommon::get_shield_info(board, shield_info);
    if (error_code != IFX_OK)
    {
        ifx_error_set(error_code);
    }
}

//----------------------------------------------------------------------------

ifx_Float_t ifx_avian_get_chirp_time(const ifx_Avian_Device_t* handle, const ifx_Avian_Config_t* config)
{
    IFX_ERR_BRV_NULL(handle, NAN);
    IFX_ERR_BRV_NULL(config, NAN);
    return handle->get_device_calc_chirp_time(config);
}

//----------------------------------------------------------------------------

ifx_Float_t ifx_avian_get_tx_power(ifx_Avian_Device_t* handle, uint8_t tx_antenna)
{
    IFX_ERR_BRV_NULL(handle, NAN);

    auto get_tx_power = [&handle, &tx_antenna]() {
        return handle->get_tx_power(tx_antenna);
    };

    return rdk::RadarDeviceCommon::exec_func<ifx_Float_t>(get_tx_power, NAN);
}

//----------------------------------------------------------------------------

void ifx_avian_configure_shapes(ifx_Avian_Device_t* handle, ifx_Avian_Shape_Set_t* shape_set)
{
    IFX_ERR_BRK_NULL(handle);
    IFX_ERR_BRK_NULL(shape_set);

    auto configure_shapes = [&handle, &shape_set]() {
        handle->configure_shapes(*shape_set);
    };

    rdk::RadarDeviceCommon::exec_func(configure_shapes);
}