include(CheckLibraryExists)

set(SDK_RECORDING_SOURCES
    Recording.cpp
    RecordingList.cpp)

set(SDK_RECORDING_HEADERS
    Recording.h
    Recording.hpp
    RecordingList.hpp)

add_library(sdk_recording_obj OBJECT ${SDK_RECORDING_SOURCES} ${SDK_RECORDING_HEADERS})
target_include_directories(sdk_recording_obj PUBLIC ..)

target_link_libraries(sdk_recording_obj PUBLIC nlohmann_json)

# Check if it is necessary to link against stdc++fs
if(CMAKE_CXX_COMPILER_LOADED)
  if(CMAKE_COMPILER_IS_CLANG)
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
      target_link_libraries(sdk_recording_obj PRIVATE stdc++fs)
    endif()
  elseif(CMAKE_COMPILER_IS_GNUCXX AND NOT MINGW)
    if(NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.3 AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
      target_link_libraries(sdk_recording_obj PRIVATE stdc++fs)
    endif()
  endif()
endif()

add_library(sdk_recording SHARED ../dummy.c)
target_link_libraries(sdk_recording PUBLIC sdk_base sdk_recording_obj nlohmann_json)
target_link_libraries(sdk_recording PRIVATE sdk_util_obj)
