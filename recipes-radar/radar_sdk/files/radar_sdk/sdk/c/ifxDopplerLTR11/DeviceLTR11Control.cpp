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
 * @file DeviceLTR11.cpp
 *
 * @brief Implementation for supporting externs LTR11
 */
#include <cstddef> // for std::nullptr_t

#include "DeviceLTR11Control.h"
#include "internal/LTR11Device.hpp"
#include "internal/LTR11DeviceDummy.hpp"

#include "ifxBase/Defines.h"
#include "ifxBase/Log.h"
#include "ifxBase/Mem.h"
#include "ifxBase/List.h"
#include "ifxBase/Uuid.h"
#include "ifxBase/internal/List.hpp"
#include "ifxRadarDeviceCommon/internal/RadarDeviceCommon.hpp"

#include "DeviceLTR11Control.h"
#include "internal/LTR11Device.hpp"

namespace
{
ifx_LTR11_Device_t* openBoard(std::unique_ptr<BoardInstance> board)
{
    auto open_ltr11 = [&board]() {
        return new LTR11Device(std::move(board));
    };

    return rdk::RadarDeviceCommon::exec_func<ifx_LTR11_Device_t* >(open_ltr11, nullptr);
}
} // end of anonymous namespace


ifx_List_t* ifx_ltr11_get_list(void)
{
    auto selector = [](const ifx_Radar_Sensor_List_Entry_t& entry) {
        return rdk::RadarDeviceCommon::sensor_is_ltr11(entry.sensor_type);
    };

    auto list = rdk::RadarDeviceCommon::get_list(selector);
    return ifx_list_from_vector(list);
}

ifx_LTR11_Device_t* ifx_ltr11_create()
{
    auto selector = [](const ifx_Radar_Sensor_List_Entry_t& entry) {
        return entry.sensor_type == IFX_BGT60LTR11AIP;
    };

    auto board = rdk::RadarDeviceCommon::open(selector);
    return ::openBoard(std::move(board));
}

ifx_LTR11_Device_t* ifx_ltr11_create_by_uuid(const char* uuid)
{
    IFX_ERR_BRV_NULL(uuid, nullptr);

    uint8_t uuid_array[16] = {};
    if (!ifx_uuid_from_string(uuid, uuid_array))
    {
        ifx_error_set(IFX_ERROR_ARGUMENT_INVALID);
        return nullptr;
    }

    auto board = rdk::RadarDeviceCommon::open_by_uuid(uuid_array);
    return ::openBoard(std::move(board));
}

ifx_LTR11_Device_t* ifx_ltr11_create_dummy()
{
    auto create_dummy = []() {
        return new LTR11DeviceDummy();
    };

    return rdk::RadarDeviceCommon::exec_func<ifx_LTR11_Device_t* >(create_dummy, nullptr);
}

void ifx_ltr11_destroy(ifx_LTR11_Device_t* handle)
{
    delete handle;
}

void ifx_ltr11_set_config(ifx_LTR11_Device_t* handle, const ifx_LTR11_Config_t* config)
{
    IFX_ERR_BRK_NULL(handle);
    IFX_ERR_BRK_NULL(config);

    auto set_config = [&handle , &config]() {
        handle->setConfig(config);
    };

    rdk::RadarDeviceCommon::exec_func(set_config);
}

void ifx_ltr11_get_config(ifx_LTR11_Device_t* handle, ifx_LTR11_Config_t* config)
{
    IFX_ERR_BRK_NULL(handle);
    IFX_ERR_BRK_NULL(config);

    auto get_config = [&handle, &config] {
        *config = handle->getConfig();
    };

    rdk::RadarDeviceCommon::exec_func(get_config);
}

void ifx_ltr11_get_config_defaults(ifx_LTR11_Device_t* handle, ifx_LTR11_Config_t* config)
{
    *config = handle->getDefaultConfig();
}

void ifx_ltr11_get_limits(ifx_LTR11_Device_t* handle, ifx_LTR11_Config_Limits_t* limits)
{
    IFX_ERR_BRK_NULL(handle);
    return handle->getLimits(limits);
}


void ifx_ltr11_start_acquisition(ifx_LTR11_Device_t* handle)
{
    IFX_ERR_BRK_NULL(handle);

    auto start_acquisition = [&handle]() {
        handle->startAcquisition();
    };

    rdk::RadarDeviceCommon::exec_func(start_acquisition);
}

void ifx_ltr11_stop_acquisition(ifx_LTR11_Device_t* handle)
{
    IFX_ERR_BRK_NULL(handle);

    auto stop_acquisition = [&handle]() {
        handle->stopAcquisition();
    };

    rdk::RadarDeviceCommon::exec_func(stop_acquisition);
}

ifx_Vector_C_t* ifx_ltr11_get_next_frame(ifx_LTR11_Device_t* handle, ifx_Vector_C_t* frame_data, ifx_LTR11_Metadata_t* metadata)
{
    return ifx_ltr11_get_next_frame_timeout(handle, frame_data, metadata, 1000);
}

ifx_Vector_C_t* ifx_ltr11_get_next_frame_timeout(ifx_LTR11_Device_t* handle, ifx_Vector_C_t* frame_data, ifx_LTR11_Metadata_t* metadata, uint16_t timeout_ms)
{
    IFX_ERR_BRN_NULL(handle);

    auto get_next_frame_timeout = [&handle, &frame_data, &metadata, &timeout_ms]() {
        return handle->getNextFrame(frame_data, metadata, timeout_ms);
    };

    return rdk::RadarDeviceCommon::exec_func<ifx_Vector_C_t* >(get_next_frame_timeout, NULL);
}

void ifx_ltr11_register_dump_to_file(ifx_LTR11_Device_t* handle, const char* filename)
{
    IFX_ERR_BRK_NULL(handle);
    IFX_ERR_BRK_NULL(filename);
    handle->dumpRegisters(filename);
}

const ifx_Radar_Sensor_Info_t* ifx_ltr11_get_sensor_information(ifx_LTR11_Device_t* handle)
{
    IFX_ERR_BRV_NULL(handle, nullptr);
    return handle->get_sensor_info();
}

const ifx_Firmware_Info_t* ifx_ltr11_get_firmware_information(ifx_LTR11_Device_t* handle)
{
    IFX_ERR_BRV_NULL(handle, nullptr);
    return handle->get_firmware_info();
}

ifx_Float_t ifx_ltr11_get_active_mode_power(ifx_LTR11_Device_t *handle, const ifx_LTR11_Config_t *config)
{
    IFX_ERR_BRV_NULL(handle, 0);
    IFX_ERR_BRV_NULL(config, 0);

    auto get_active_mode_power = [&handle, &config](){
        return handle->getActiveModePower(config);
    };
    return rdk::RadarDeviceCommon::exec_func<ifx_Float_t>(get_active_mode_power, 0);
}

uint32_t ifx_ltr11_get_sampling_frequency(ifx_LTR11_Device_t* handle, ifx_LTR11_PRT_t prt_index)
{
    IFX_ERR_BRV_NULL(handle, 0);

    auto get_sampling_freq = [&handle, &prt_index]() {
        return handle->getSamplingFrequency(prt_index);
    };
    return rdk::RadarDeviceCommon::exec_func<uint32_t>(get_sampling_freq, 0);
}

bool ifx_ltr11_check_config(ifx_LTR11_Device_t *handle, const ifx_LTR11_Config_t *config)
{
    IFX_ERR_BRV_NULL(handle, false);

    auto check_config = [&handle, &config] {
        return handle->checkConfig(config);
    };

    return rdk::RadarDeviceCommon::exec_func<bool>(check_config, false);
}