if(MSVC)
    # Visual Studio
    # /EHsc: synchronous exception handling
    set(CMAKE_CXX_FLAGS_INIT                "/W3 /EHsc /std:c++17")
    set(CMAKE_CXX_FLAGS_DEBUG_INIT          "/D_DEBUG /MTd /Zi /Ob0 /Od /RTC1")
    set(CMAKE_CXX_FLAGS_MINSIZEREL_INIT     "/MT /O1 /Ob1 /D NDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE_INIT        "/MT /O2 /Ob2 /D NDEBUG")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "/MT /Zi /O2 /Ob1 /D NDEBUG")

	# suppress inheritance via dominance warning for virtual inheritance (4250)
	# suppress "The enum type is unscoped. Prefer 'enum class' over 'enum'" (26812)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_INIT} /wd4250 /wd26812")
else()
    # gcc and clang
    set(CMAKE_CXX_FLAGS_INIT                "${CMAKE_CXX_FLAGS_INIT} -std=c++17 -Wall -Wextra -Wno-unused-parameter -Werror=vla")
    set(CMAKE_CXX_FLAGS_DEBUG_INIT          "-g")
    set(CMAKE_CXX_FLAGS_ASAN_INIT           "-g -fno-omit-frame-pointer -fsanitize=address -fsanitize=undefined")
    set(CMAKE_CXX_FLAGS_MINSIZEREL_INIT     "-Os -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE_INIT        "-O3 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "-O2 -g")

    # For clang on MinGW use lld as linker. lld is faster when it comes to
    # linking C++ object files with a large number of symbols compared to ld.
    # (This is actually a bug/limitation on MinGW.) Also, using sanitizers
    # works on MinGW only with clang in combination with lld.
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND MINGW)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=lld")

        # lld by default uses multiple threads. As there are bugs in the
        # winpthread that lld uses, using lld with multiple threads might
        # result in lld hanging indefinetly. To avoid this, we disable
        # multi-threading support for lld.
        add_link_options("LINKER:/threads:no")
    endif()
endif()
