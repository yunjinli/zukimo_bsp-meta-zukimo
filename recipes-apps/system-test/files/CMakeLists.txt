cmake_minimum_required(VERSION 3.10)

set (module system_test)

# specify the sources to compile
file(GLOB tp_sources main.cpp system_test.cpp)

add_executable(${module} ${tp_sources})

target_link_libraries(${module} rt pthread m gtest)

install(TARGETS ${module} DESTINATION bin)
install(PROGRAMS start_system_test.sh DESTINATION bin)