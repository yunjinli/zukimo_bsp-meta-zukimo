set(SDK_DOPPLER_LTR11_SOURCES
    DeviceLTR11Control.cpp
    internal/LTR11Device.cpp
    internal/LTR11DeviceBase.cpp
    internal/LTR11DeviceDummy.cpp
    internal/LTR11RegisterManipulator.cpp)

set(SDK_DOPPLER_LTR11_HEADERS
    DopplerLTR11.h
    DeviceLTR11Control.h
    Types.h
    internal/LTR11Device.hpp
    internal/LTR11DeviceBase.hpp
    internal/LTR11DeviceConstants.hpp
    internal/LTR11DeviceDummy.hpp
    internal/LTR11RegisterManipulator.hpp)

add_library(sdk_doppler_ltr11_obj OBJECT ${SDK_DOPPLER_LTR11_SOURCES} ${SDK_DOPPLER_LTR11_HEADERS})
target_include_directories(sdk_doppler_ltr11_obj PUBLIC ..)
target_link_libraries(sdk_doppler_ltr11_obj PUBLIC sdk_base_obj sdk_radar_device_common_obj)
target_link_libraries(sdk_doppler_ltr11_obj PRIVATE strata_static)

add_library(sdk_doppler_ltr11 SHARED ../dummy.c)
target_link_libraries(sdk_doppler_ltr11 PRIVATE sdk_radar_device_common_obj)
target_link_libraries(sdk_doppler_ltr11 PUBLIC sdk_base sdk_doppler_ltr11_obj strata_static)