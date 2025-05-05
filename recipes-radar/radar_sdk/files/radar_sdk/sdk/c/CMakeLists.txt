#cmake_minimum_required(VERSION 3.12)

include(CheckLibraryExists)
include(Macros)

option(SDK_ENABLE_LOGS   "enable logging with debug log level " OFF)

# export symbols when building
add_definitions(-Dradar_sdk_EXPORTS=1)

if(SDK_ENABLE_LOGS)
    add_definitions(-DIFX_LOG_SEVERITY_DEBUG=1)
endif()

# Check if it is necessary to link against libm
check_library_exists(m sqrt "" HAS_LIBM)

subdirlist(directories ${CMAKE_CURRENT_LIST_DIR})
foreach(directory ${directories})
	if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${directory}/CMakeLists.txt")
		add_subdirectory(${directory})
	endif()
endforeach()
