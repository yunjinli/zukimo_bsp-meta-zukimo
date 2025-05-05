set(SDK_RADAR_SOURCES
    AngleCapon.c
    AngleMonopulse.c
    DBF.c
    DBF.h
    DeInterleaver.cpp
    PeakSearch.c
    PresenceSensing.c
    RangeAngleImage.c
    RangeDopplerMap.c
    RangeSpectrum.c
    SpectrumAxis.cpp)

set(SDK_RADAR_HEADERS
    AngleCapon.h
    AngleMonopulse.h
    DBF.h
    DeInterleaver.hpp
    PeakSearch.h
    PresenceSensing.h
    Radar.h
    RangeAngleImage.c
    RangeAngleImage.h
    RangeDopplerMap.h
    RangeSpectrum.h
    SpectrumAxis.cpp
    SpectrumAxis.h
    internal/DeInterleaver.h)

add_library(sdk_radar_obj OBJECT ${SDK_RADAR_SOURCES} ${SDK_RADAR_HEADERS})
target_include_directories(sdk_radar_obj PUBLIC ..)
target_link_libraries(sdk_radar_obj PUBLIC sdk_base_obj)
target_link_libraries(sdk_radar_obj PRIVATE sdk_algo_obj)
target_link_libraries(sdk_radar_obj INTERFACE sdk_avian_obj)

add_library(sdk_radar SHARED ../dummy.c)
target_link_libraries(sdk_radar PUBLIC sdk_radar_obj sdk_base sdk_algo sdk_avian)
target_link_libraries(sdk_radar INTERFACE sdk_avian)