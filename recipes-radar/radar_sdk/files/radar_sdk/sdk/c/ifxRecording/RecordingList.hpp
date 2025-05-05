#pragma once
#include <string>
#include <vector>
#include <exception>
#include <fstream>
#include <cstring>

#include "ifxBase/List.h"
#include "ifxBase/Exception.hpp"
#include "Recording.h"

#if (_WIN32 && defined _MSC_VER && _MSC_VER < 1920)
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#include <filesystem>
namespace fs = std::filesystem;
#endif

class RecordingList
{
public:
	RecordingList(const std::string &path);
	std::vector<ifx_Recording_Entry_t> getRecordingList();

protected:
	static int32_t getIndex(const std::string &dir_name);
	static ifx_Recording_Type_t getType(const std::string &dir_name);
	std::vector<std::string> getDirectoriesNames() const;
	const char* getParentPath() const;

private:
	fs::path m_directory_path;
	std::vector<std::string> m_directories;
	std::vector<ifx_Recording_Entry_t> m_recording_list;
};
