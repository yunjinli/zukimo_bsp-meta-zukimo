set(SDK_AVIAN_SOURCES
    ConstantWaveControl.cpp
    DeviceCalc.c
    DeviceConfig.cpp
    DeviceControl.cpp
    DeviceControlAvian.cpp
    DeviceControlWrappers.cpp
    DummyRadarDevice.cpp
    Metrics.cpp
    RadarDevice.cpp
    RadarDeviceBase.cpp
    RadarDeviceErrorTranslator.cpp
    RawDataFifo.cpp
    RecordingRadarDevice.cpp)

set(SDK_AVIAN_HEADERS
    Avian.h
    ConstantWaveControl.h
    DeviceConfig.h
    DeviceControl.h
    DeviceControlHelper.hpp
    Metrics.cpp
    Metrics.h
    Shapes.h
    internal/DeviceCalc.h
    internal/DeviceControlAvian.hpp
    internal/DummyControlPort.hpp
    internal/DummyRadarDevice.hpp
    internal/RadarDevice.hpp
    internal/RadarDeviceBase.hpp
    internal/RadarDeviceErrorTranslator.hpp
    internal/RawDataFifo.hpp
    internal/RecordingRadarDevice.hpp)

add_library(sdk_avian_obj OBJECT ${SDK_AVIAN_SOURCES} ${SDK_AVIAN_HEADERS})
target_include_directories(sdk_avian_obj PUBLIC ..)

target_link_libraries(sdk_avian_obj PRIVATE lib_avian_static nlohmann_json strata_static)

add_library(sdk_avian SHARED ../dummy.c)

target_link_libraries(sdk_avian PUBLIC sdk_avian_obj sdk_radar_device_common_obj)
target_link_libraries(sdk_avian PUBLIC sdk_base sdk_recording sdk_util_obj)
