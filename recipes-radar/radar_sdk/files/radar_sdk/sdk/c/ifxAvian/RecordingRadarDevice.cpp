/* ===========================================================================
** Copyright (C) 2021 - 2022 Infineon Technologies AG
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

#include "internal/RecordingRadarDevice.hpp"
#include "ifxAvian/internal/RadarDeviceErrorTranslator.hpp"
#include "ifxBase/Exception.hpp"
#include "ifxRecording/Recording.h"
#include "ifxUtil/internal/NpyReader.hpp"

#include "ifxRadarDeviceCommon/internal/RadarDeviceCommon.hpp"

#include <string_view>
#include <utility>
#include <array>
#include <thread>
/*
==============================================================================
	2. LOCAL DEFINITIONS
==============================================================================
*/

namespace {

    // Config.json specific names for Avian devices
    inline constexpr std::string_view CONFIG_NAME   {"device_config"};
    inline constexpr std::string_view CONFIG_TYPE   {"fmcw_single_shape"};
    inline constexpr std::string_view DEVICE_INFO   {"device_info"};
    inline constexpr std::string_view DEVICE_NAME   {"device_name"};
    inline constexpr float normalized_adc_scale{ 1.f / 4095 };

    /*  Parse the json and get a json_object for a given key.
        If there is no such key, throws invalid_json. */
    inline const auto& get_json_object(const nlohmann::json& json_object, const std::string_view key)
    {
        try {
            if (!json_object.contains(key)) {
                throw rdk::exception::invalid_json_key();
            }
            return json_object.at(key);
        }
        catch (nlohmann::json::exception&) {
            throw rdk::exception::invalid_json();
        }
    }

    /*  Parse the json and get an object of type T for a given key.
        If there is no such key, throws invalid_json. */
    template <typename T>
    inline T get_json_by_key(const nlohmann::json& json_object, const std::string_view key) {
        try {
            if (!json_object.contains(key)) {
                throw rdk::exception::argument_invalid();
            }
            return json_object[key].get<T>();
        }
        catch (nlohmann::json::exception&) {
            throw rdk::exception::invalid_json();
        }
    }

    // Parses the config.json and gets the single shape configuration, if there is any
    inline const auto& get_fmcw_single_shape_config(const nlohmann::json& config) {
        return get_json_object(get_json_object(config, CONFIG_NAME), CONFIG_TYPE);
    }

    // Gets proper mimo_mode value
    inline auto get_mimo_mode(const nlohmann::json& config) {
        if (!config.contains("mimo_mode"))
            return IFX_MIMO_OFF;

        try {
            auto property = get_json_object(config, "mimo_mode");
            // Support setting mimo mode as a number in config
            if (property.is_number()) {
                property = property.get<int>();
                if (property == IFX_MIMO_OFF) {
                    return IFX_MIMO_OFF;
                } else if (property == IFX_MIMO_TDM) {
                    return IFX_MIMO_TDM;
                }
            }
            else {
                property = property.get<std::string_view>();
                if (property == "off") {
                    return IFX_MIMO_OFF;
                } else if (property == "tdm") {
                    return IFX_MIMO_TDM;
                }
            }
        }
        catch (nlohmann::json::exception&) {
            throw rdk::exception::invalid_json_value();
        }
        throw rdk::exception::invalid_json_value();
    }

    //Gets proper mask value
    inline auto get_antenna_mask(const nlohmann::json& config, const std::string_view key)
    {
        uint32_t bitmask = 0;
        try {
            const auto& property = get_json_object(config, key);
            if (!property.is_array()) {
                throw rdk::exception::invalid_json();
            }

            for (const auto& it : property)
            {
                if (!it.is_number_unsigned()) {
                    throw rdk::exception::invalid_json();
                }

                auto value = it.get<uint64_t>();
                if (value < 1 || value > 32) {
                    throw rdk::exception::invalid_json();
                }

                bitmask |= 1 << (value-1);
            }
        }
        catch (nlohmann::json::exception&) {
            throw rdk::exception::invalid_json_value();
        }
        return bitmask;
    }

    // Gets Avian Device_Type, if proper string is given, otherwise throws index_out_of_bounds
    inline Avian::Device_Type get_device_type(const nlohmann::json& config) {
        if (!config.contains(DEVICE_INFO))
            return Avian::Device_Type::BGT60TR13C;

        const std::string device_name = get_json_object(get_json_object(config, DEVICE_INFO), DEVICE_NAME).get<std::string>();
        const ifx_Radar_Sensor_t sensor = rdk::RadarDeviceCommon::string_to_sensor(device_name);
        if (sensor == IFX_AVIAN_UNKNOWN || !rdk::RadarDeviceCommon::sensor_is_avian(sensor))
            throw rdk::exception::invalid_json_key();
        return Avian::Device_Type(sensor);
    }
}

/*
==============================================================================
   6. LOCAL FUNCTIONS
==============================================================================
*/

RecordingRadarDevice::RecordingRadarDevice(ifx_Recording_t* recording, bool correct_timing)
: m_recording(recording), m_correct_timing(correct_timing)
{
    constexpr uint32_t HP_CUTOFF_HZ_DEFAULT = 80000; // 80kHZ
    constexpr uint32_t AAF_CUTOFF_HZ_DEFAULT = 500000; // 500kHz
    constexpr uint32_t SAMPLE_RATE_HZ_DEFAULT = 1000000; // 1MHz

    if (!m_recording) {
        throw rdk::exception::argument_null();
    }

    // Recording data must contain real cubes (frames)
    auto info = m_recording->get_npy_reader()->get_info();
    if ((info.dtype[0] == 'c') || (info.shape.size() != 4)) {
        throw rdk::exception::file_invalid();
    }

    const auto& config = m_recording->get_config();
    auto device_type = get_device_type(config);
    m_driver = std::make_unique<Avian::Driver>(m_avian_dummy_port, device_type);
    if (!m_driver) {
        throw rdk::exception::device_not_supported();
    }
    initialize();

    const auto& fmcw_single_shape_config = get_fmcw_single_shape_config(config);

    m_config.sample_rate_Hz = fmcw_single_shape_config.contains("sample_rate_Hz")
        ? get_json_by_key<uint32_t>(fmcw_single_shape_config, "sample_rate_Hz")
        : SAMPLE_RATE_HZ_DEFAULT;
    m_config.rx_mask = get_antenna_mask(fmcw_single_shape_config, "rx_antennas");
    m_config.tx_mask = get_antenna_mask(fmcw_single_shape_config, "tx_antennas");
    m_config.tx_power_level = get_json_by_key<uint32_t>(fmcw_single_shape_config, "tx_power_level");
    m_config.if_gain_dB = get_json_by_key<uint32_t>(fmcw_single_shape_config, "if_gain_dB");
    m_config.start_frequency_Hz = fmcw_single_shape_config.contains("start_frequency_Hz")
        ? get_json_by_key<uint64_t>(fmcw_single_shape_config, "start_frequency_Hz")
        : get_json_by_key<uint64_t>(fmcw_single_shape_config, "lower_frequency_Hz");
    m_config.end_frequency_Hz = fmcw_single_shape_config.contains("end_frequency_Hz")
        ? get_json_by_key<uint64_t>(fmcw_single_shape_config, "end_frequency_Hz")
        : get_json_by_key<uint64_t>(fmcw_single_shape_config, "upper_frequency_Hz");
    m_config.num_chirps_per_frame = get_json_by_key<uint32_t>(fmcw_single_shape_config, "num_chirps_per_frame");
    m_config.num_samples_per_chirp = get_json_by_key<uint32_t>(fmcw_single_shape_config, "num_samples_per_chirp");
    m_config.chirp_repetition_time_s = static_cast<ifx_Float_t>(get_json_by_key<float>(fmcw_single_shape_config, "chirp_repetition_time_s"));
    m_config.frame_repetition_time_s = static_cast<ifx_Float_t>(get_json_by_key<float>(fmcw_single_shape_config, "frame_repetition_time_s"));
    m_config.hp_cutoff_Hz = fmcw_single_shape_config.contains("hp_cutoff_Hz")
        ? get_json_by_key<uint32_t>(fmcw_single_shape_config, "hp_cutoff_Hz")
        : HP_CUTOFF_HZ_DEFAULT;
    m_config.aaf_cutoff_Hz = fmcw_single_shape_config.contains("aaf_cutoff_Hz")
        ? get_json_by_key<uint32_t>(fmcw_single_shape_config, "aaf_cutoff_Hz")
        : AAF_CUTOFF_HZ_DEFAULT;
    m_config.mimo_mode = get_mimo_mode(fmcw_single_shape_config);
}

void RecordingRadarDevice::get_config(ifx_Avian_Config_t& config)
{
    config = m_config;
}

void RecordingRadarDevice::set_config(const ifx_Avian_Config_t& config)
{
    throw rdk::exception::not_supported();
}

void RecordingRadarDevice::start_acquisition()
{
    if(!m_acquisition_started)
    {
        m_time_acquisition_started = std::chrono::high_resolution_clock::now();
        m_acquisition_started = true;
    }
}

void RecordingRadarDevice::stop_acquisition()
{
    m_acquisition_started = false;
    m_last_frame = 0;
}

ifx_Cube_R_t* RecordingRadarDevice::get_next_frame(ifx_Cube_R_t* frame, uint16_t timeout_ms)
{
    assert(m_recording);

    auto info = m_recording->get_npy_reader()->get_info();

    // Throw an error, when we are out of recording data and calling next_frame:
    if (m_last_frame >= info.shape[0]) {
        throw rdk::exception::end_of_file();
    }

    if (!frame) {
        // allocate memory for a frame
        frame = ifx_cube_create_r(info.shape[1], info.shape[2], info.shape[3]);
        if (!frame) {
            throw rdk::exception::memory_allocation_failed();
        }
    }
    else {
        // check if dimensions of cube frame are correct
        if (IFX_CUBE_ROWS(frame) != info.shape[1] ||
            IFX_CUBE_COLS(frame) != info.shape[2] ||
            IFX_CUBE_SLICES(frame) != info.shape[3]) {
            throw rdk::exception::dimension_mismatch();
        }
    }

    if (m_correct_timing) {

        // Only if correct_timing is true we need a timestamp where acquisition was started.
        // Thus, if it was not started we should start it with first get_next_frame() call.
        // If correct_timing is false, we return the next frame regardless of acquisition state.
        // This means starting acquisition with a handle without correct timing is no-op.
        start_acquisition();

        using std::chrono::duration;
        using std::chrono::duration_cast;
        using std::chrono::milliseconds;

        auto time_now = std::chrono::high_resolution_clock::now();
        auto time_since_acquisition_started_ms = duration_cast<milliseconds>(time_now - m_time_acquisition_started);
        double frame_available_after = (m_last_frame + 1) * m_config.frame_repetition_time_s;
        auto time_for_available_frame_ms = duration_cast<milliseconds>(duration<double>(frame_available_after));
        auto time_left_to_available_frame_ms = duration_cast<milliseconds>(time_for_available_frame_ms - time_since_acquisition_started_ms);

        // When we need to wait for a frame to be available
        if (time_left_to_available_frame_ms.count() > 0) {
            if (time_left_to_available_frame_ms.count() > timeout_ms) {
                throw rdk::exception::timeout();
            }
            std::this_thread::sleep_for(time_left_to_available_frame_ms);
        }        
    }

    try {
        m_recording->get_npy_reader()->get_cube_r_at(m_last_frame, frame, normalized_adc_scale);
    }
    catch (NPYException&) {
        throw rdk::exception::file_invalid();
    }

    m_last_frame++;

    return frame;
}