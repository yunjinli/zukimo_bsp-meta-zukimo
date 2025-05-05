#include "RecordingList.hpp"
#include "ifxBase/Error.h"

RecordingList::RecordingList(const std::string& path) : m_directory_path(path)
{
	auto directories = getDirectoriesNames();
	if (directories.empty())
	{
		throw rdk::exception::empty_directory();
	}
	for (const auto& dir : directories)
	{
		auto index = getIndex(dir);

		if (index < 0) {
			throw rdk::exception::index_out_of_bounds();
		}
		ifx_Recording_Entry_t entry;
		entry.index = index;
		entry.type = getType(dir);
		entry.parent_path = getParentPath();
		m_recording_list.push_back(entry);

	}
}

std::vector<ifx_Recording_Entry_t> RecordingList::getRecordingList()
{
	return m_recording_list;
}

int32_t RecordingList::getIndex(const std::string& dir_name)
{
	int result = -1;
	if (dir_name.size() > 3 && dir_name.at(dir_name.size() - 3) == '_')
	{
		std::string number = dir_name.substr(dir_name.size() - 2, dir_name.size());

		for (auto& digit : number)
		{

			if (!::isdigit(digit))
			{
				return result;
			}
		}
		result = std::stoi(number);

	}
	return result;
}

ifx_Recording_Type_t RecordingList::getType(const std::string& dir_name)
{
	if (dir_name.size() <= 3 || dir_name.at(dir_name.size() - 3) != '_')
	{
		return ifx_Recording_Type_t::IFX_RECORDING_UNKNOWN;
	}

	std::string result = dir_name.substr(0, dir_name.size() - 3);

	if (result == "RadarIfxAvian")
	{
		return ifx_Recording_Type_t::IFX_RECORDING_AVIAN;
	}

	return ifx_Recording_Type_t::IFX_RECORDING_UNKNOWN;
}


std::vector<std::string> RecordingList::getDirectoriesNames() const
{
	auto main_directory = fs::directory_entry(m_directory_path);
	std::vector<std::string> result_names;

	if (!fs::exists(main_directory))
	{
		throw fs::filesystem_error::exception();
	}

	for (const fs::directory_entry& dir_entry : fs::directory_iterator(main_directory))
	{
		result_names.push_back(dir_entry.path().filename().string());
	}

	return result_names;
}

const char* RecordingList::getParentPath() const
{
	size_t size = m_directory_path.string().size() + 1;
	char* result = new char[size]();
#if defined(_MSC_VER)
	strcpy_s(result, size, m_directory_path.string().c_str());
#else
	strcpy(result, m_directory_path.string().c_str());
#endif

	return result;
}

