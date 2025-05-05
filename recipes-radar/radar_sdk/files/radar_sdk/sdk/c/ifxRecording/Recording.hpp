#pragma once

#include "Recording.h"

#include <memory>
#include <string>
#include "nlohmann/json.hpp"
#include "ifxUtil/internal/NpyReader.hpp"
#include "ifxUtil/Mmap.h"

struct Recording
{
public:
	Recording(const std::string& path,
			  ifx_Recording_Mode_t mode,
			  ifx_Recording_Type_t type,
			  uint32_t index);
	Recording(const Recording&) = delete;
	Recording(Recording&&) = delete;
	Recording& operator=(const Recording&) = delete;
	Recording& operator=(Recording&&) = delete;
	~Recording();

	const std::unique_ptr<NpyReader>& get_npy_reader() const {
		return m_npy_reader;
	}

	//const std::unique_ptr<NpyWriter>& get_npy_writer(); // return m_npy_write - to be implemented later

	const nlohmann::json& get_config() const {
		return m_config;
	}

	const nlohmann::json& get_meta() const {
		return m_meta;
	}

	const std::string& get_version() const {
		return m_format_version;
	}

	const ifx_Recording_Type_t& get_type() const {
		return m_type;
	}

	const ifx_Recording_Mode_t& get_mode() const {
		return m_mode;
	}

private:
	//std::unique_ptr<NpyWriter> m_npy_write; // to be implemented later
	ifx_Recording_Mode_t 		m_mode;
	ifx_Recording_Type_t 		m_type;
	nlohmann::json		 		m_meta{};
	nlohmann::json		 		m_config{};
	std::string			 		m_format_version{};
	std::unique_ptr<NpyReader>	m_npy_reader{nullptr};
	ifx_MMAP_t* 				m_mmap_handle{nullptr};
};