cmake_minimum_required(VERSION 3.1)

set(MIO_DIR ${RDK_3RD_PARTY_DIR}/libs/mio)

set(MIO_HEADERS mio.hpp)

add_library(mio INTERFACE)
#target_sources(mio INTERFACE ${MIO_HEADERS})
target_sources(mio INTERFACE ${MIO_DIR}/mio.hpp)
target_include_directories(mio INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
