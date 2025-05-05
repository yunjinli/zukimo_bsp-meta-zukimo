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

#include "Recording.h"
#include "Recording.hpp"
#include "RecordingList.hpp"
#include "ifxBase/Log.h"
#include "ifxBase/Exception.hpp"
#include "ifxBase/Error.h"
#include <fstream>

/*
==============================================================================
	2. DEFINITIONS
==============================================================================
*/

namespace {
	/* Recording directory specific file and directory names definitions
	   according to ifxDaq documentation: */
	inline const std::string AVIAN_DIR_NAME		{"/RadarIfxAvian"};
	inline const std::string META_FILENAME		{"/meta.json"};
	inline const std::string CONFIG_FILENAME	{"/config.json"};
	inline const std::string VERSION_FILENAME	{"/format.version"};
	inline const std::string RADAR_DATA_FILENAME{"/radar.npy"};

	/* Helper functions to open and read data from file and parse it as string or json. */
	inline std::ifstream open_file(const std::string& full_path) {
		std::ifstream ifs{};

		ifs.open(full_path, std::ios_base::in);

		if ((!ifs.good() || !ifs.is_open())) {
			throw rdk::exception::opening_file();
		}
		return ifs;
	}

	inline nlohmann::json read_json(const std::string& full_path) {
		auto ifs = open_file(full_path);

		auto parsed_json = nlohmann::json::parse(ifs);

		ifs.close();
		return parsed_json;
	}

	inline std::string read_format_version(const std::string& full_path) {
		auto ifs = open_file(full_path);

		std::string line{};
		std::getline(ifs, line);

		ifs.close();
		return line; 
	}

	inline std::string get_full_path(const std::string& path,
									 ifx_Recording_Mode_t mode,
									 ifx_Recording_Type_t type,
									 uint32_t index) {
		/* In directory under path we have recordings in catalogs
		named "RadarIfx<RadarType>_XX" where XX is a recording index,
		e.g. RadarIfxAvian_01. The function might be replaced in the future
		by a function that gets a list of directories (and their contents)*/
		std::string full_path{path};
		switch(type) {
			case IFX_RECORDING_AVIAN:
				full_path += AVIAN_DIR_NAME;
				break;
			case IFX_RECORDING_UNKNOWN:
			default:
				throw rdk::exception::not_supported();
		}
		full_path += (index < 10 ? "_0" : "_") + std::to_string(index);
		return full_path;
	}
}

/*
==============================================================================
   3. LOCAL TYPES
==============================================================================
*/

Recording::Recording(const std::string& path,
					 ifx_Recording_Mode_t mode,
					 ifx_Recording_Type_t type,
					 uint32_t index)
					 : m_mode{mode}, m_type{type}
{
	// Note: Only Read mode is available now
	if (mode != IFX_RECORDING_READ_MODE) {
		throw rdk::exception::not_supported();
	}
	if (path.empty()) {
		throw rdk::exception::argument_invalid();
	}

	auto full_path = get_full_path(path, mode, type, index);

	m_meta = read_json(full_path + META_FILENAME);
	m_config = read_json(full_path + CONFIG_FILENAME);
	m_format_version = read_format_version(full_path + VERSION_FILENAME);

	size_t mmap_length{0};
	m_mmap_handle = ifx_mmap_create((full_path + RADAR_DATA_FILENAME).c_str(), &mmap_length);

	if((m_mmap_handle == nullptr) || !mmap_length) {
		throw rdk::exception::opening_file();
	}

	m_npy_reader = std::make_unique<NpyReader>(ifx_mmap_const_data(m_mmap_handle), mmap_length);
}

Recording::~Recording() {
	ifx_mmap_destroy(m_mmap_handle);
}


/*
==============================================================================
   7. EXPORTED FUNCTIONS
==============================================================================
*/

ifx_Recording_t* ifx_recording_create(const char* path,
									  ifx_Recording_Mode_t mode,
									  ifx_Recording_Type_t type,
									  int32_t index)
{
	IFX_ERR_BRV_NULL(path, NULL);
	ifx_Recording_t* recording{nullptr};

	try {
		recording = new Recording(path, mode, type, index);
	}
	catch (nlohmann::json::parse_error& e)
	{
		(void)e;
		IFX_LOG_ERROR("Could not parse file: " + e.what());
		ifx_error_set(IFX_ERROR_ARGUMENT_INVALID);
	}
	catch(std::ios_base::failure& e)
	{
		(void)e;
		IFX_LOG_ERROR("Could not open/read file: " + e.what());
		ifx_error_set(IFX_ERROR);
	}
	catch (const std::bad_alloc& e)
	{
		(void)e;
		IFX_LOG_ERROR("Memory allocation failed: " + e.what());
		ifx_error_set(IFX_ERROR_MEMORY_ALLOCATION_FAILED);
	}
	catch (const rdk::exception::exception& e)
	{
		(void)e;
		IFX_LOG_ERROR("RDK error: " + e.what());
		ifx_error_set(e.error_code());
	}
	catch(const std::runtime_error& e)
	{
		(void)e;
		IFX_LOG_ERROR("Runtime error: " + e.what());
		ifx_error_set(IFX_ERROR);
	}
	catch (const std::exception& e)
	{
		(void)e;
		IFX_LOG_ERROR("Standard library error: " + e.what());
		ifx_error_set(IFX_ERROR);
	}
	catch (...)
	{
		ifx_error_set(IFX_ERROR);
	}

	return recording;
}

void ifx_recording_destroy(ifx_Recording_t* recording)
{
	delete recording;
}

const char* ifx_recording_get_format_version(ifx_Recording_t* recording)
{
	IFX_ERR_BRV_NULL(recording, NULL);
	return recording->get_version().c_str();
}

ifx_List_t* ifx_recording_list(const char* path)
{

	auto free_list = [](void* ptr_void)
	{
		auto* ptr = static_cast<ifx_Recording_Entry_t*>(ptr_void);
		delete[] ptr->parent_path;
		delete ptr;
	};

	ifx_List_t* result = ifx_list_create(free_list);

	IFX_ERR_BRV_NULL(path, NULL);

	try
	{
		RecordingList list(path);

		for (auto& entry : list.getRecordingList())
		{
			auto* local_entry = new ifx_Recording_Entry_t();
			*local_entry = entry;
			ifx_list_push_back(result, static_cast<void*>(local_entry));
		}
	}
	catch (const rdk::exception::empty_directory& e) {
		(void)e;
		IFX_LOG_ERROR("Directory is empty: " + e.what());
		ifx_error_set(IFX_ERROR_EMPTY_DIRECTORY);
	}
	catch (const rdk::exception::index_out_of_bounds& e) {
		(void)e;
		IFX_LOG_ERROR("The directory has the wrong postfix number: " + e.what());
		ifx_error_set(IFX_ERROR_INDEX_OUT_OF_BOUNDS);
	}
	catch (const std::runtime_error& e) {
		(void)e;
		IFX_LOG_ERROR("Could not read file or directory: " + e.what());
		ifx_error_set(IFX_ERROR);
	}
	catch (const std::exception& e) {
		(void)e;
		IFX_LOG_ERROR("Standard library error: " + e.what());
		ifx_error_set(IFX_ERROR);
	}
	catch (...) {
		ifx_error_set(IFX_ERROR);
	}

	return result;
}