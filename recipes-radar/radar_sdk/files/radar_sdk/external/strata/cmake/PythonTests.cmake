
function(strata_add_python_test name type wrapper_target)
    add_test(NAME ${name}
        COMMAND ${Python3_EXECUTABLE} -m unittest discover -s "${CMAKE_CURRENT_SOURCE_DIR}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
    set_tests_properties(${name} PROPERTIES LABELS ${type})

    set(python_path $<TARGET_FILE_DIR:${wrapper_target}>)
    list(APPEND python_path $ENV{PYTHONPATH})
    if(UNIX)
        string(REPLACE ";" ":" python_path "${python_path}")
    endif()
    set_tests_properties(${name} PROPERTIES ENVIRONMENT "PYTHONPATH=${python_path}")
endfunction()
