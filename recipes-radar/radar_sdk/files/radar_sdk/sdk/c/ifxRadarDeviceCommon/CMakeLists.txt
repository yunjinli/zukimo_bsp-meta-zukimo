add_library(sdk_radar_device_common_obj OBJECT
    AcquisitionState.cpp
    RadarDeviceCommon.cpp
    RadarDeviceCommon.h
    internal/AcquisitionState.hpp
    internal/RadarDeviceCommon.hpp
    internal/RfShieldEeprom.cpp
    internal/RfShieldEeprom.hpp)
target_include_directories(sdk_radar_device_common_obj PUBLIC ..)

target_link_libraries(sdk_radar_device_common_obj PUBLIC strata_static)
target_link_libraries(sdk_radar_device_common_obj PRIVATE lib_avian_static)