# daqkit tool converter
set(DAQKIT_SOURCES 
    src/DaqKitConverter.cpp
    src/converter.cpp)

set(DAQKIT_HEADERS 
    src/DaqKitConverter.hpp)

add_executable(daqkit_converter ${DAQKIT_SOURCES} ${DAQKIT_HEADERS})
target_compile_definitions(daqkit_converter PRIVATE radar_sdk_EXPORTS=1)
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        set(CXX_FILESYSTEM_LIBRARIES "stdc++fs")
endif()
set_property(TARGET daqkit_converter PROPERTY CXX_STANDARD 17)
target_link_libraries(daqkit_converter PUBLIC sdk_radar argparse sdk_util_obj app_common ${CXX_FILESYSTEM_LIBRARIES})