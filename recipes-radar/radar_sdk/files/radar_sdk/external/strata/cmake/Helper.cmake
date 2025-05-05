
function(add_subdirectory_optional dir)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${dir}/CMakeLists.txt")
        add_subdirectory(${dir})
    endif()
endfunction()


# from cmake 3.19 onwards, we can finally do the following
# and later simply use target_link_libraries()
# (but we still don't for backwards compatibility)
#
#add_library(${name} INTERFACE ${ARGN})
#target_include_directories(${name} INTERFACE ${ARGN})
#target_compile_definitions(${name} INTERFACE ${ARGN})

function(add_interface_library name)
    add_custom_target(${name} SOURCES ${ARGN})
endfunction()

function(interface_target_include_directories name)
    set_target_properties(${name} PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${ARGN})
endfunction()

function(interface_target_compile_definitions name)
    set_target_properties(${name} PROPERTIES INTERFACE_COMPILE_DEFINITIONS ${ARGN})
endfunction()


# Linking to object libraries and to our workaround interface libraries has to be implemented manually:

function(target_link_libraries_helper target)
    # determine the link type for object dependencies
    get_target_property(target_type ${target} TYPE)
    if(target_type STREQUAL OBJECT_LIBRARY)
        set(LINK_TYPE PUBLIC)
    else()
        set(LINK_TYPE PRIVATE)
    endif()

    foreach(item ${ARGN})
        # populating the interface properties is the same for both interface and object libraries

        get_target_property(INCLUDE_DIRS ${item} INTERFACE_INCLUDE_DIRECTORIES)
        if(NOT INCLUDE_DIRS STREQUAL INCLUDE_DIRS-NOTFOUND)
            target_include_directories(${target} PUBLIC ${INCLUDE_DIRS})
        endif()

        get_target_property(COMPILE_DEFS ${item} INTERFACE_COMPILE_DEFINITIONS)
        if(NOT COMPILE_DEFS STREQUAL COMPILE_DEFS-NOTFOUND)
            target_compile_definitions(${target} PUBLIC ${COMPILE_DEFS})
        endif()

        get_target_property(LINK_LIBRARIES ${item} INTERFACE_LINK_LIBRARIES)
        if(NOT LINK_LIBRARIES STREQUAL LINK_LIBRARIES-NOTFOUND)
            target_link_libraries(${target} PUBLIC ${LINK_LIBRARIES})
        endif()

        # if we have an object dependency, we need to handle its source elements
        get_target_property(type ${item} TYPE)
        if(type STREQUAL OBJECT_LIBRARY)
            get_target_property(SOURCES ${item} SOURCES)
            foreach(source ${SOURCES})
                if ("${source}" MATCHES "^\\$\\<TARGET_OBJECTS:")
                    target_sources(${target} ${LINK_TYPE} ${source})
                endif()
            endforeach()

            target_sources(${target} ${LINK_TYPE} $<TARGET_OBJECTS:${item}>)
        endif()
    endforeach()
endfunction()
