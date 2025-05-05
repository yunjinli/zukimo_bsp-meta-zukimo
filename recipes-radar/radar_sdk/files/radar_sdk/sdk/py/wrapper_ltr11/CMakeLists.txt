find_package(Python3)

# When cmake creates files for build tool, the file pyproject.toml is created
# in the source directory with the correct SDK version.
# When the target wheel-ltr11 is built the files of the wrapper, and the files
# LICENSE.txt, setup.py, pyproject.toml files are copied to the build directory
# together with the radar_sdk library. Then the python wheel is created.

if(${Python3_FOUND})
    # create Error.py
    execute_process(COMMAND ${Python3_EXECUTABLE} ${RDK_DIR}/scripts/py/generate_ifxError.py -i ${RDK_SDK_DIR}/c/ifxBase/Error.h -o ${RDK_SDK_DIR}/py/wrapper_ltr11/src/ifxDopplerLTR11/Error.py RESULT_VARIABLE ret ERROR_QUIET)

    execute_process(COMMAND ${Python3_EXECUTABLE} -c "import wheel" RESULT_VARIABLE ret ERROR_QUIET)
    if(ret AND NOT ret EQUAL "0")
        message("Python library wheel not found: Cannot build python wheels")
    else()
        set(WRAPPER_DIR "${CMAKE_SOURCE_DIR}/sdk/py/wrapper_ltr11/")
        set(WRAPPER_BUILD_DIR "${CMAKE_BINARY_DIR}/wheel_ltr11")

        # create pyproject.toml file in source directory
        configure_file("${WRAPPER_DIR}/pyproject.toml.in" "${WRAPPER_DIR}/pyproject.toml" @ONLY)

        add_custom_target(wheel-ltr11 ALL
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${WRAPPER_BUILD_DIR}                                  # remove wheel_ltr11 in build directory
        COMMAND ${CMAKE_COMMAND} -E make_directory ${WRAPPER_BUILD_DIR}                                    # create wheel_ltr11 in build directory
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${WRAPPER_DIR}/src ${WRAPPER_BUILD_DIR}/src             # copy src directory
        COMMAND ${CMAKE_COMMAND} -E copy ${WRAPPER_DIR}/LICENSE.txt ${WRAPPER_BUILD_DIR}/LICENSE.txt       # copy LICSENSE.txt file
        COMMAND ${CMAKE_COMMAND} -E copy ${WRAPPER_DIR}/setup.py ${WRAPPER_BUILD_DIR}/setup.py             # copy setup.py file
        COMMAND ${CMAKE_COMMAND} -E copy ${WRAPPER_DIR}/pyproject.toml ${WRAPPER_BUILD_DIR}/pyproject.toml # copy pyproject.toml file
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:radar_sdk> ${WRAPPER_BUILD_DIR}/src/ifxDopplerLTR11 # copy radar SDK library
        COMMAND ${Python3_EXECUTABLE} -m build -w ${WRAPPER_BUILD_DIR} -n                                  # build Python wheel for ltr11 wrapper
        DEPENDS radar_sdk)

        # do not build wheel-ltr11 by default
        set_target_properties(wheel-ltr11 PROPERTIES EXCLUDE_FROM_ALL 1)
    endif()
endif()
