cmake_minimum_required(VERSION 3.12)

set(RDK_3RD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rd_party)
set(RDK_CMAKE_DIR     ${CMAKE_CURRENT_SOURCE_DIR}/scripts/cmake)
set(RDK_SDK_DIR       ${CMAKE_CURRENT_SOURCE_DIR}/sdk)

set(CMAKE_MODULE_PATH "${RDK_CMAKE_DIR};${CMAKE_MODULE_PATH}")

# set default values for C and C++ flags (flags can be overwritten by targets)
set(CMAKE_USER_MAKE_RULES_OVERRIDE     ${RDK_CMAKE_DIR}/c_flags_overrides.cmake)
set(CMAKE_USER_MAKE_RULES_OVERRIDE_CXX ${RDK_CMAKE_DIR}/cxx_flags_overrides.cmake)

project(rdk)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# determine version from git
set(VERSION_UPDATE_FROM_GIT FALSE)
include(GetVersionFromGitTag)
include(TestBigEndian)

test_big_endian(SDK_TARGET_PLATFORM_BIG_ENDIAN)
if(SDK_TARGET_PLATFORM_BIG_ENDIAN)
    add_definitions(-DSDK_TARGET_PLATFORM_BIG_ENDIAN)
endif()

# build all targets using position independent code (PIC)
# It is important that this definition comes before any subdirectory is
# included or any target is defined such that all targets are compiled
# using position independent code.
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# on MinGW don't add the prefix "lib" to shared and static libraries
if(MINGW)
   set(CMAKE_SHARED_LIBRARY_PREFIX "")
   set(CMAKE_STATIC_LIBRARY_PREFIX "")
endif()

# strata and lib_avian
set(STRATA_BUILD_WRAPPERS OFF CACHE BOOL "" FORCE)
add_subdirectory("./external/strata" EXCLUDE_FROM_ALL)
add_subdirectory("./external/lib_avian" EXCLUDE_FROM_ALL)

# 3rd party
add_subdirectory("${RDK_3RD_PARTY_DIR}/libs/")

# SDK C library
add_subdirectory("./sdk/c/")

# applications
add_subdirectory("./apps/c/")

# tools
add_subdirectory("./tools/")

# python wrappers for Avian and BGT60LTR11AIP
add_subdirectory("./sdk/py/wrapper_avian")
add_subdirectory("./sdk/py/wrapper_ltr11")