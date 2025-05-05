# shared library of radar_sdk
add_library(radar_sdk SHARED ../dummy.c)
if(WIN32 OR MINGW OR MSYS)
    # on Windows the library should not have the prefix lib
    set_target_properties(radar_sdk PROPERTIES PREFIX "")
endif()

target_link_libraries(radar_sdk PUBLIC sdk_base_obj sdk_algo_obj sdk_recording_obj sdk_util_obj sdk_doppler_ltr11_obj sdk_radar_obj sdk_avian_obj sdk_radar_device_common_obj)