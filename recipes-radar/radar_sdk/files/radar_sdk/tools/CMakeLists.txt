include(Macros)

subdirlist(directories ${CMAKE_CURRENT_LIST_DIR})
foreach(directory ${directories})
	if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${directory}/CMakeLists.txt")
		add_subdirectory(${directory})
	endif()
endforeach()