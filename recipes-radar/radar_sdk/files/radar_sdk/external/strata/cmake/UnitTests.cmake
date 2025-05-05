
function(strata_add_unit_test name)
    add_executable(${name} ${name}.cpp)
    add_test(NAME ${name} COMMAND ${name} --gtest_output=xml:${STRATA_UNIT_TEST_OUTPUT_DIR}/${name}.xml)

    set_tests_properties(${name} PROPERTIES LABELS "UNIT")
    target_include_directories(${name} PUBLIC "${STRATA_DIR}/tests")
    target_link_libraries(${name} gmock_main)
    foreach(dep ${ARGN})
        get_target_property(type ${dep} TYPE)
        if(type MATCHES "(OBJECT_LIBRARY|UTILITY)")
            target_link_libraries_helper(${name} ${dep})
        else()
            target_link_libraries(${name} ${dep})
        endif()
    endforeach()

    set(env_path $<TARGET_FILE_DIR:gmock_main>)
    foreach(arg IN LISTS ARGN)
        get_target_property(target_type ${arg} TYPE)
        if(target_type STREQUAL SHARED_LIBRARY)
            # if the test links to a shared library the test needs to know where to find it
            # Therefore, the environment will be adapted so it will find the lib when the test will be executed with ctest
            list(APPEND env_path $<TARGET_FILE_DIR:${arg}>)
        endif()
    endforeach()

    list(APPEND env_path $ENV{PATH})
    if(UNIX)
        string(REPLACE ";" ":" env_path "${env_path}")
    elseif(WIN32)
        # escape the semicolon to avoid that cmake interpret the string 
        # contained in the resulting ctest file as a list
        string(REPLACE ";" "\\;" env_path "${env_path}")
    endif()
    set_tests_properties(${name} PROPERTIES ENVIRONMENT "PATH=${env_path}")

    # add unit test to global list for test handling
    set(STRATA_UNIT_TESTS ${STRATA_UNIT_TESTS} ${name} CACHE INTERNAL "")
endfunction()

function(strata_add_unit_test_with_input_files)
    set(options OPTIONAL)
    set(oneValueArgs NAME)
    set(multiValueArgs INPUT_FILES LINK_LIBRARIES)
    cmake_parse_arguments(TEST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    strata_add_unit_test(${TEST_NAME} ${TEST_LINK_LIBRARIES})
    get_target_property(BINARY_DIR ${TEST_NAME} BINARY_DIR)

    #parsing optional parameters in order to copy the input test files to working directory
    #copy each input file to output directory
    foreach(file IN LISTS TEST_INPUT_FILES)
        add_custom_command(TARGET ${TEST_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${file} "${BINARY_DIR}"
            )
    endforeach()
endfunction()
