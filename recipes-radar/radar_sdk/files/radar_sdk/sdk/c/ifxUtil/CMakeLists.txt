set(SDK_UTIL_SOURCES
    NpyReader.cpp
    Util.cpp
    Mmap.cpp)

set(SDK_UTIL_HEADERS
    Mmap.h
    Util.h)

add_library(sdk_util_obj OBJECT ${SDK_UTIL_SOURCES} ${SDK_UTIL_HEADERS})

# this macro enables safety functions with *_s postfix, if available
target_compile_definitions(sdk_util_obj PRIVATE __STDC_WANT_LIB_EXT1__=1)

target_link_libraries(sdk_util_obj PRIVATE sdk_base_obj zip mio)
