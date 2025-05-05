
unset(CMAKE_CXX_CLANG_TIDY) # do not run static analysis on contrib code (in case it was enabled before)

file(GLOB CONTRIB_FILES *.cmake)
add_custom_target(_contrib SOURCES ${CONTRIB_FILES})


if(STRATA_BUILD_TESTS)
    include(catch2.cmake)
    include(gtest.cmake)
endif()

add_subdirectory_optional(mcd_api)

include(rapidjson.cmake)

if(STRATA_BUILD_SAMPLES)
    include(qwt.cmake OPTIONAL)
endif()
