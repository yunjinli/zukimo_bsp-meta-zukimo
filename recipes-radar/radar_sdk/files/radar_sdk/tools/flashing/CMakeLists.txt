find_package(Threads REQUIRED)

# sources for the flashing library
set(FLASHLIB_SOURCES
    src/findComPort.cpp
    src/ActivateBootloader.cpp
    src/FlashRadar.cpp)

set(FLASHLIB_HEADERS
    src/findComPort.h
    src/FlashRadar.h)

# shared library of flashing library
add_library(flash_rbb_mcu7 SHARED ${FLASHLIB_SOURCES} ${FLASHLIB_HEADERS})
set_target_properties(flash_rbb_mcu7 PROPERTIES DEBUG_POSTFIX "d")
set_target_properties(flash_rbb_mcu7 PROPERTIES EXCLUDE_FROM_ALL 1)
target_include_directories(flash_rbb_mcu7 PRIVATE ../../external/strata/library/)
target_link_libraries(flash_rbb_mcu7 PRIVATE bossa)
if(WIN32 OR MINGW OR MSYS)
    target_link_libraries(flash_rbb_mcu7 PRIVATE setupapi ${CMAKE_THREAD_LIBS_INIT})
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    target_link_libraries(flash_rbb_mcu7 PUBLIC "-framework CoreFoundation -framework IOKit")
endif()

# static version of flashing library
add_library(flash_rbb_mcu7_static STATIC ${FLASHLIB_SOURCES} ${FLASHLIB_HEADERS})
target_compile_definitions(flash_rbb_mcu7_static PUBLIC FLASHLIB_BUILD_STATIC=1)
target_include_directories(flash_rbb_mcu7_static PRIVATE ../../external/strata/library/)
target_link_libraries(flash_rbb_mcu7_static PRIVATE bossa)
if(WIN32 OR MINGW OR MSYS)
    target_link_libraries(flash_rbb_mcu7_static PRIVATE setupapi ${CMAKE_THREAD_LIBS_INIT})
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    target_link_libraries(flash_rbb_mcu7_static PUBLIC "-framework CoreFoundation -framework IOKit")
endif()

# flashtool (link against static version of library to avoid dependency on shared library)
add_executable(flashtool_rbb7 src/flashtool.cpp)
target_link_libraries(flashtool_rbb7 flash_rbb_mcu7_static argparse)