function(set_resource_info target)
    set(prefix VER)
    set(options OPTIONAL)
    set(oneValueArgs)
    set(multiValueArgs FILETYPE COMPANYNAME ORIGINALFILENAME INTERNALNAME PRODUCTNAME FILEDESCRIPTION)
    cmake_parse_arguments("${prefix}" "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        set(RES_FILES
            "${STRATA_DIR}/resource/version.rc"
            )
    else()
        # also explicitly build manifest
        set(RES_FILES
            "${STRATA_DIR}/resource/version.rc"
            "${STRATA_DIR}/resource/manifest.rc"
              )
    endif()

    if (${CMAKE_RC_COMPILER} MATCHES ".*windres.*")
        # windres cannot handle quoted strings with spaces, so replace them by escape sequence
        string(REPLACE " " "\\x20" VER_COMPANYNAME ${VER_COMPANYNAME})
        string(REPLACE " " "\\x20" VER_ORIGINALFILENAME ${VER_ORIGINALFILENAME})
        string(REPLACE " " "\\x20" VER_INTERNALNAME ${VER_INTERNALNAME})
        string(REPLACE " " "\\x20" VER_PRODUCTNAME ${VER_PRODUCTNAME})
        string(REPLACE " " "\\x20" VER_FILEDESCRIPTION ${VER_FILEDESCRIPTION})
    endif()

    if(NOT VER_FILETYPE)
        get_target_property(type ${target} TYPE)
        if (${type} STREQUAL STATIC_LIBRARY)
            set(VER_FILETYPE VFT_STATIC_LIB)
        elseif(${type} STREQUAL EXECUTABLE)
            set(VER_FILETYPE VFT_APP)
        else()
            set(VER_FILETYPE VFT_DLL)
        endif()
    endif()

    target_sources(${target} PRIVATE ${RES_FILES})
    target_compile_definitions(${target} PRIVATE
        ${STRATA_VERSION_DEFINES}
        VER_FILETYPE=${VER_FILETYPE}
        VER_COMPANYNAME="${VER_COMPANYNAME}"
        VER_ORIGINALFILENAME="${VER_ORIGINALFILENAME}"
        VER_INTERNALNAME="${VER_INTERNALNAME}"
        VER_PRODUCTNAME="${VER_PRODUCTNAME}"
        VER_FILEDESCRIPTION="${VER_FILEDESCRIPTION}"
        )
    set_target_properties(${target}
        PROPERTIES
        VERSION ${STRATA_VERSION_MAJOR}.${STRATA_VERSION_MINOR}.${STRATA_VERSION_PATCH}
        )

endfunction()
