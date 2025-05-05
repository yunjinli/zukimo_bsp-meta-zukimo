set(SDK_BASE_SOURCES
    Complex.c
    Cube.c
    Error.c
    LA.c
    LA.h
    List.cpp
    Log.c
    Math.c
    Matrix.c
    Mem.c
    Util.c
    Uuid.c
    Vector.c
    Version.c)

set(SDK_BASE_HEADERS
    Base.h
    Complex.h
    Cube.h
    Defines.h
    Error.h
    Exception.hpp
    Helper.hpp
    LA.h
    List.cpp
    List.h
    Log.h
    Math.h
    Matrix.h
    Mem.h
    Types.h
    Uuid.c
    Uuid.h
    Vector.h
    Version.h
    internal/Clamping.hpp
    internal/GuardedHandle.hpp
    internal/List.hpp
    internal/Macros.h
    internal/NonCopyable.hpp
    internal/Simd.h
    internal/Util.h)

add_library(sdk_base_obj OBJECT ${SDK_BASE_SOURCES} ${SDK_BASE_HEADERS})
target_include_directories(sdk_base_obj PUBLIC ..)

if(HAS_LIBM)
    target_link_libraries(sdk_base_obj PUBLIC m)
endif()

# shared library
add_library(sdk_base SHARED ../dummy.c)
target_link_libraries(sdk_base PUBLIC sdk_base_obj)
