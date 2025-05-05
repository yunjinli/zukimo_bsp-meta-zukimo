# ===========================================================================
# Copyright (C) 2021-2022 Infineon Technologies AG
# All rights reserved.
# ===========================================================================
#
# ===========================================================================
# This document contains proprietary information of Infineon Technologies AG.
# Passing on and copying of this document, and communication of its contents
# is not permitted without Infineon's prior written authorization.
# ===========================================================================

cmake_minimum_required(VERSION 3.10)

project(lib_avian)

###############################################################################
# Options
###############################################################################

# By default the StrataPort is not included in the avian library because this
# introduces the Strata library as dependency. Including StrataPort may be
# explicitly enabled.
# Anyway, if the Strata library is detected in the environment, StrataPort is
# automatically included.
option(AVIAN_INCLUDE_STRATA_PORT
       "Include StrataPort implementation in Avian library"
       OFF)

if (TARGET strata_static)
    set(AVIAN_INCLUDE_STRATA_PORT ON)
endif()

if (AVIAN_INCLUDE_STRATA_PORT)
    message(STATUS "Lib Avian includes StrataPort.")
endif()

###############################################################################
# Source Files
###############################################################################
# All source and header files are collected.
file(GLOB_RECURSE source_files
     *.hpp
     *.cpp
     *.h)

# If including StrataPort was not selected the according files are removed from
# the source file list.
if (NOT AVIAN_INCLUDE_STRATA_PORT)
    file(GLOB_RECURSE strata_port_files
         ifxAvian_StrataPort.*)
    list(REMOVE_ITEM source_files ${strata_port_files})
endif()

# The file system structure of source files is preserved in generated IDE
# project.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}
             PREFIX "Source Files"
             FILES ${source_files})

###############################################################################
# Target #1: Static Library
###############################################################################
# The static library is built from all source files. The file
# VersionHistory.txt is added to make it appear in the IDE project, it does not
# contribute to the build.
add_library(lib_avian_static STATIC
            ${source_files}
            VersionHistory.txt)

# The directory containing API headers is announced for targets that link
# against this library. The directory containing source files is only used
# when building lib_avian, but not announced to depending projects.
target_include_directories(lib_avian_static
                           PUBLIC include
                           PRIVATE src)

# If including StrataPort was selected, the Strata library is also required.
if (AVIAN_INCLUDE_STRATA_PORT)
    target_link_libraries(lib_avian_static
                          PRIVATE strata_static)
endif()

# The name of the artifact is kept compact. It's necessary to call it "lib",
# because the file extension will indicate that the target is a library.
set_target_properties(lib_avian_static PROPERTIES
                      OUTPUT_NAME avian)
