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

/**
 * @internal
 * @file RecordingRadarDevice.hpp
 *
 * @brief Defines the structure for the Recording Radar Device Controller Module
 * created out of the saved Common File Format Recording.
*/

#pragma once

#include "ifxAvian/internal/DummyRadarDevice.hpp"
#include "ifxUtil/internal/NpyReader.hpp"
#include "ifxRecording/Recording.hpp"
#include <memory>
#include <chrono>

struct RecordingRadarDevice : public ifx_Dummy_Radar_Device_s
{
    NONCOPYABLE(RecordingRadarDevice);

    RecordingRadarDevice(ifx_Recording_t* recording, bool correct_timing);
    RecordingRadarDevice(const RecordingRadarDevice&&) = delete;
    RecordingRadarDevice& operator=(RecordingRadarDevice&&) = delete;
    ~RecordingRadarDevice() override = default;

    void set_config(const ifx_Avian_Config_t& config) override;
    void get_config(ifx_Avian_Config_t& config) override;
    void start_acquisition() override;
    void stop_acquisition() override;
    ifx_Cube_R_t* get_next_frame(ifx_Cube_R_t* frame, uint16_t timeout_ms) override;
private:
    Recording*  m_recording{nullptr};
    uint64_t    m_last_frame{0};
    bool        m_correct_timing{false};
    bool        m_acquisition_started{false};
    std::chrono::high_resolution_clock::time_point m_time_acquisition_started{};
};