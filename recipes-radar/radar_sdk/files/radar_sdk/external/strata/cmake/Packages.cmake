function(strata_package_add_library name type)
    add_library(${name} ${type} ${ARGN})
    target_include_directories(${name} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${STRATA_INCLUDE_DIRS}"
        )
    target_compile_definitions(${name} PUBLIC
        ${STRATA_PLATFORM_DEFINES}
        )

    target_link_libraries(${name} PUBLIC ${STRATA_LIBRARY_DEPENDENCIES})

    if(${type} STREQUAL OBJECT)
        set(LINK_TYPE PUBLIC)
    else()
        set(LINK_TYPE PRIVATE)
    endif()

    target_sources(${name} ${LINK_TYPE} ${STRATA_LIBRARY_OBJECTS})
endfunction()


function(strata_package_install name)
    set(INSTALL_BINARY_DIR "${STRATA_BINARY_DIR}/${STRATA_TARGET_PLATFORM}/x${STRATA_BITNESS}/${name}")
    install(TARGETS ${name} DESTINATION ${INSTALL_BINARY_DIR})

    get_target_property(type ${name} TYPE)
    if (${type} STREQUAL STATIC_LIBRARY)
        list(APPEND STRATA_LIBRARY_DEPENDENCIES ${ARGN})
        install(CODE "file(WRITE ${INSTALL_BINARY_DIR}/deps.txt \"${STRATA_LIBRARY_DEPENDENCIES}\")")
    endif()
endfunction()
