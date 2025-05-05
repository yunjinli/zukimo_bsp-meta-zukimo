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

#include <cmath>
#include "DaqKitConverter.hpp"
#include "ifxBase/Version.h"

#if (_WIN32 && _MSC_VER < 1920)
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#include <filesystem>
namespace fs = std::filesystem;
#endif

namespace Infineon::Converter
{

	void DaqKitConverter::loadFromFiles(const std::string &config_file, const std::string &matrix_file, const std::string &output_dir_path)
	{
		auto directory_entry = fs::directory_entry(config_file);

		if (!fs::exists(directory_entry))
		{
			throw DaqKitException("Radar configuration json file is missing\n");
		}

		directory_entry = fs::directory_entry(matrix_file);
		if (!fs::exists(directory_entry))
		{
			throw DaqKitException("Radar recorded data file is missing\n");
		}

		m_output_dir_path = output_dir_path;
		m_json_config.load_from_file(config_file, 0, nullptr);
		m_json_config.get_config_fmcw_single_shape(&m_device_config);
		loadRecordedData(matrix_file);
	}

	void DaqKitConverter::saveToDaqKitFormat()
	{
		std::string output_dir;
		fs::path output_path;

		for (uint8_t i = 0; i < 99; i++)
		{
			if (i < 10)
			{
				output_dir = "RadarIfxAvian_0" + std::to_string(i);
			}
			else
			{
				output_dir = "RadarIfxAvian_" + std::to_string(i);
			}

			fs::path save_path;

			if (!m_output_dir_path.empty())
			{
				save_path.append(m_output_dir_path);
			}

			save_path.append(output_dir);

			auto directory_entry = fs::directory_entry(save_path);

			if (!fs::exists(directory_entry))
			{
				try
				{
					fs::create_directories(save_path);
				}
				catch (const fs::filesystem_error &e)
				{
					throw DaqKitException(std::string("The saving path file is corrupted or ").append(e.what()));
				}

				output_path = save_path;
				break;
			}
		}

		if (output_path.empty())
		{
			throw DaqKitException("RadarIfxAvian_[00-99] multiple directories exists, please clean up / delete directories.");
		}

		output_path.make_preferred();

		saveConfigJson((output_path / m_output_files.config_json).string());
		saveMeta((output_path / m_output_files.meta).string());
		saveNpy((output_path / m_output_files.npy).string());
		saveFormat((output_path / m_output_files.format_version).string());
		saveHostMeta((output_path.parent_path() / m_output_files.host_meta_json).string());
	}

	void DaqKitConverter::loadRecordedData(const std::string &recorded_data_file)
	{
		std::ifstream file;
		file.open(recorded_data_file);

		if (!file.is_open())
		{
			throw DaqKitException("Cannot open file with recorded data.");
		}

		m_date_captured = getFileCreationTime(recorded_data_file);

		std::string str;
		uint64_t recording_lines = 0;
		try
		{
			while (std::getline(file, str))
			{
				recording_lines++;
			}

			try
			{
				m_recording.reserve(recording_lines);
			}
			catch (const std::length_error &e)
			{
				std::cout << std::string("The size of the record data file is too big - memory allocation error.\n").append(e.what());
			}

			file.clear();
			file.seekg(0, std::ios::beg);

			const auto unnormalized_uint16 = std::pow(2, 12) - 1;

			while (std::getline(file, str))
			{
				if (!str.empty() && str.front() != '\r')
				{
					float p = std::strtof(str.c_str(), nullptr);
					m_recording.push_back(static_cast<uint16_t>(std::round(p * unnormalized_uint16)));
				}
			}
		}
		catch (std::ios_base::failure &e)
		{
			throw DaqKitException(std::string("Error reading recorded file\n").append(e.what()));
		}

		file.close();
	}

	void DaqKitConverter::saveMeta(const std::string &meta_file) const
	{
		nlohmann::json j;

		try {
			j["uuid"] = ""; // uuid is skip right now.
			j["sdk_version"] = ifx_sdk_get_version_string_full();
			j["adc_resolution"] = adc_resolution;
		}
		catch (nlohmann::json::parse_error& e)
		{
			(void)e;
			IFX_LOG_ERROR("Could not parse file: " + e.what());
		}
		catch (nlohmann::json::exception & e)
		{
			(void)e;
			IFX_LOG_ERROR("JSON Exception: " + e.what());
		}

		std::ofstream file;

		file.open(meta_file);
		if (!file.is_open())
		{
			throw DaqKitException("Cannot open file for writing meta.json data");
		}

		try
		{
			file << std::setw(4) << j;
		}
		catch (std::ios_base::failure &e)
		{
			throw DaqKitException(std::string("Error writing to meta.json file - ").append(e.what()));
		}
		catch (...)
		{
			throw DaqKitException("Unknown error, writing operation with meta.json file is not successful.");
		}
		file.close();
	}

	void DaqKitConverter::saveNpy(const std::string &npy_file)
	{
		bool fortran_mode = false;
		bool is_complex = false;

		auto antennas = getAntenaNumber();
		auto frames = getFramesNumber();
		auto chirp_per_frame = m_device_config.num_chirps_per_frame;
		auto samples_per_chirp = m_device_config.num_samples_per_chirp;

		const auto num_shapes = 4;
		uint64_t shape[num_shapes] = {frames, antennas, chirp_per_frame, samples_per_chirp};

		if (!ifxu_npy_write_raw(npy_file.c_str(), m_recording.data(), is_complex, fortran_mode, shape, num_shapes))
		{
			throw DaqKitException("Could not open file, or file header assembly failure.");
		};
	}

	void DaqKitConverter::saveFormat(const std::string &format_version_file) const
	{

		std::ofstream file;
		file.open(format_version_file);
		if (!file.is_open())
		{
			throw DaqKitException("Cannot open file for writing format data");
		}

		try
		{
			file << m_format_version;
		}
		catch (std::ios_base::failure &e)
		{
			throw DaqKitException(std::string("Error writing to format file - ").append(e.what()));
		}
		catch (...)
		{
			throw DaqKitException("Unknown error, writing operation with format.version file is not successful.");
		}
		file.close();
	}

	void DaqKitConverter::saveConfigJson(const std::string &output_config_filename)
	{
		m_json_config.save_to_file(output_config_filename);
	}

	void DaqKitConverter::saveHostMeta(const std::string &filename)
	{
		nlohmann::json j;

		try {
			j["_comment"] = comment_json;
			j["ifxdaq_version"] = ifxdaq_version;
			j["username"];
			j["hostname"];
			j["os"];
			j["os_release"];
			j["os_version"];
			j["machine"];
			j["python_version"];
			j["python_implementation"];
			j["date_captured"] = m_date_captured;
		}
		catch (nlohmann::json::parse_error& e)
		{
			(void)e;
			IFX_LOG_ERROR("Could not parse file: " + e.what());
		}
		catch (nlohmann::json::exception& e)
		{
			(void)e;
			IFX_LOG_ERROR("JSON Exception: " + e.what());
		}

		std::ofstream file;
		file.open(filename);
		if (!file.is_open())
		{
			throw std::runtime_error("Cannot open '" + filename + "' file for writing.");
		}

		try
		{
			file << std::setw(4) << j;
		}
		catch (...)
		{
			throw std::runtime_error("Error writing to file '" + filename + "'.");
		}
		file.close();
	}

	uint32_t DaqKitConverter::getFramesNumber() const
	{
		auto total_num_of_samples = m_recording.size();
		auto num_of_antena = getAntenaNumber();
		auto divide_block = num_of_antena * m_device_config.num_chirps_per_frame * m_device_config.num_samples_per_chirp;
		if (divide_block == 0)
		{
			throw DaqKitException("Divide by zero in frame number counting.");
		}
		auto result = total_num_of_samples / divide_block;

		return static_cast<uint32_t>(result);
	}

	uint32_t DaqKitConverter::getAntenaNumber() const
	{
		return static_cast<uint32_t>(std::bitset<sizeof(uint32_t)>(m_device_config.rx_mask).count());
	}

	std::string DaqKitConverter::getFileCreationTime(const std::string& file_name) const
	{
		fs::file_time_type file_time = fs::last_write_time(file_name);
		std::time_t t_time = to_time_t(file_time);
		char buf[sizeof("1970-01-01T00:00:00Z")];

		#if defined(__WIN32__) || defined(_WIN32)
		std::tm gmt{};
		auto err = gmtime_s(&gmt, &t_time);
		if (err)
		{
			return "null";
		}
		std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", &gmt);
		#elif defined(__linux__)
		std::tm* gmt = gmtime(&t_time);
		std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", gmt);
		#endif

		return std::string{ buf };
		}

}//namespace Infineon::Converter