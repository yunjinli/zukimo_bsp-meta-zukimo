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

#pragma once
#include <json.hpp>
#include <string>
#include <fstream>
#include <exception>
#include <iomanip>
#include <iostream>
#include <bitset>
#include <sstream>
#include <chrono>

#include "ifxUtil/Util.h"

namespace Infineon::Converter
{

    template <typename TimePoint>
    std::time_t to_time_t(TimePoint tp)
    {
        using std::chrono::time_point_cast;
        using std::chrono::system_clock;
        auto sctp = time_point_cast<system_clock::duration>(tp - TimePoint::clock::now() + system_clock::now());
        return std::chrono::system_clock::to_time_t(sctp);
    }

    const std::string format_version{"1.0.0"};
    const std::string adc_resolution{"12"};
    const std::string comment_json{"recording was converted using the converter tool."};
    const std::string ifxdaq_version{"0.1.0"};

    // Output DaqKit filenames
    struct DaqKitFileNames
    {
        const std::string config_json{"config.json"};
        const std::string meta{"meta.json"};
        const std::string npy{"radar.npy"};
        const std::string format_version{"format.version"};
        const std::string host_meta_json{"meta.json"};
    };

    /// Exception thrown by DaqKitConverter class
    class DaqKitException final : public std::exception
    {
    public:
        explicit DaqKitException(std::string what) : m_what(std::move(what)) {}

        const char *what() const noexcept override
        {
            return m_what.c_str();
        }

    private:
        std::string m_what;
    };

    /**
     * \brief Class for converting row data ifx_Float_t vector and Radar configuration json to DaqKit format.
     *
     * This class read from disk input files recorded by app_recorder:
     * 'recording' - included float ASCII streamed values
     * 'recording_config.json' - radar parameters
     * and save it to DaqKit format
     *  └───RadarIfxAvian_xx            - This is the directory name, where xx is the number 00-99.
                format.version          - The version of the actual format.
                meta.json               - Radar information
                radar.npy               - This is the standard binary file format in NumPy for persisting a single arbitrary NumPy array on disk.
                                          The format stores all of the shapes and type information (ifx_Float_t) necessary to reconstruct the array.
                recording_config.json   - Device configure information.
        └───meta.json                   - Information about host system and tools version.
     *
     */
    class DaqKitConverter
    {
    public:
        DaqKitConverter() = default;

        /**
         * \brief Get data from files.
         *
         * \param [in] config_file      The radar configuration used for the recording, JSON file format.
         * \param [in] matrix_file      Row data recoding which is a text file with one number in ASCII per line.
         * \param [in] output_dir_path  Direct output path [Optional]
         */
        void loadFromFiles(const std::string &config_file, const std::string &matrix_file, const std::string &output_dir_path = "");

        /**
         * \brief Save included files to Daq Kit format on disk to the directory where the executable file is, or in the direct path.
         *
         */
        void saveToDaqKitFormat();

    private:
        void loadRecordedData(const std::string &data_file);
        void saveMeta(const std::string &meta_file) const;
        void saveNpy(const std::string &npy_file);
        void saveFormat(const std::string &format_version_file) const;
        void saveConfigJson(const std::string &output_config_filename);
        void saveHostMeta(const std::string &filename);
        uint32_t getFramesNumber() const;
        uint32_t getAntenaNumber() const;
        std::string getFileCreationTime(const std::string &file_name) const;

        ifxJsonConfiguration m_json_config{};
        std::vector<uint16_t> m_recording{};
        std::string m_format_version{format_version};
        DaqKitFileNames m_output_files{};
        std::string m_output_dir_path{};
        ifx_Avian_Config_t m_device_config{};
        std::string m_date_captured{};
    };
}