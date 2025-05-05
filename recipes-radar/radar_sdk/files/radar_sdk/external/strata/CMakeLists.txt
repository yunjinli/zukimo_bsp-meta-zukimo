option(STRATA_PACKAGES_AUTO_DETECT "find and build additional packages automatically" ON)
option(STRATA_SHARED_LIBRARY "build shared library" ON)
option(STRATA_BUILD_ANALYZE_CODE "run static code analyzer during build" OFF)
option(STRATA_BUILD_WRAPPERS "try to build wrappers for Matlab, Python, etc." ON)
option(STRATA_BUILD_SAMPLES "build sample applications" OFF)
option(STRATA_BUILD_TOOLS "build tool applications" OFF)
option(STRATA_BUILD_TESTS "build integration and unit tests" OFF)
option(STRATA_BUILD_WARNINGS_AS_ERRORS "make build fail if compiler emits any warning" OFF)
if(STRATA_BUILD_TESTS)
    option(STRATA_BUILD_RUN_UNIT_TESTS "run unit tests after a build" OFF)
endif()

# if STRATA_MULTIPLE_PYTHON_WRAPPER_VERSIONS is enabled pybind11MultiVersion and the conan python package will be used for the build
option(STRATA_MULTIPLE_PYTHON_WRAPPER_VERSIONS "build the python wrapper for multiple python versions at once" OFF)
option(STRATA_USE_CONAN_UNIVERSAL "use the universal library from Conan" OFF)

if (NOT STRATA_USE_CONAN_UNIVERSAL)
    set(QT_CREATOR_SKIP_CONAN_SETUP True)
endif()


#set(STRATA_PACKAGES "") # STRATA_PACKAGES_AUTO_DETECT has to be disabled, since it overwrites this list


cmake_minimum_required(VERSION 3.5.1)
project(Strata)
include(Strata.cmake)
include(../local_settings.cmake OPTIONAL)

if(STRATA_USE_CONAN_UNIVERSAL)
    strata_include(Conan)
endif()

strata_include(Format)
add_format_targets(format "*.?pp;*.h" library samples tests tools wrappers packages)

strata_include(Analyze)
add_analyze_targets(analyze "*.cpp" library)


add_subdirectory_optional(contrib)

add_subdirectory(library)

if(STRATA_BUILD_WRAPPERS)
    include(wrappers/matlab.cmake)
    include(wrappers/python.cmake)

    add_subdirectory(wrappers)
endif()
if(STRATA_BUILD_TESTS)
    set(STRATA_INTEGRATION_TEST_OUTPUT_DIR "${PROJECT_BINARY_DIR}/tests/output/integration_tests")
    set(STRATA_UNIT_TEST_OUTPUT_DIR "${PROJECT_BINARY_DIR}/tests/output/unit_tests")

    add_subdirectory(tests)
endif()

if(STRATA_BUILD_SAMPLES)
    add_subdirectory(samples)
endif()
if(STRATA_BUILD_TOOLS)
    add_subdirectory(tools)
endif()


add_subdirectory_optional(packages)


if(STRATA_BUILD_TESTS)
    add_custom_target(unit_tests
        SOURCES tests/dummy
        DEPENDS ${STRATA_UNIT_TESTS}
        )
    add_custom_command(TARGET unit_tests
        POST_BUILD
        COMMAND ${CMAKE_CTEST_COMMAND}
        ARGS -L "UNIT"
        )

    if(STRATA_BUILD_RUN_UNIT_TESTS)
        add_custom_target(unit_tests_all ALL
            DEPENDS unit_test_output_dir
            DEPENDS ${STRATA_UNIT_TESTS}
            )
        add_custom_command(TARGET unit_tests_all
            POST_BUILD
            COMMAND ${CMAKE_CTEST_COMMAND}
            ARGS -L "UNIT" --stop-on-failure --output-on-failure --quiet
            )
    endif()
endif()
