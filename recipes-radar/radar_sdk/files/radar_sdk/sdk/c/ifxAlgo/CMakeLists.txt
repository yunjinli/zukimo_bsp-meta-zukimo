set(SDK_ALGO_SOURCES
    2DMTI.c
    DBSCAN.c
    FFT.c
    MTI.c
    OSCFAR.c
    PreprocessedFFT.c
    Signal.c
    Window.c)

set(SDK_ALGO_HEADERS
    2DMTI.h
    Algo.h
    DBSCAN.h
    FFT.h
    MTI.h
    OSCFAR.h
    PreprocessedFFT.h
    Signal.h
    Window.h)

add_library(sdk_algo_obj OBJECT ${SDK_ALGO_SOURCES} ${SDK_ALGO_HEADERS})

target_include_directories(sdk_algo_obj PUBLIC ..)

target_link_libraries(sdk_algo_obj PRIVATE muFFT)
target_include_directories(sdk_algo_obj PRIVATE ${FFT_INCLUDE_DIRS})
target_compile_definitions(sdk_algo_obj PRIVATE ${FFT_LIB_DEFINE})

add_library(sdk_algo SHARED ../dummy.c)
target_link_libraries(sdk_algo PUBLIC sdk_algo_obj sdk_base)
