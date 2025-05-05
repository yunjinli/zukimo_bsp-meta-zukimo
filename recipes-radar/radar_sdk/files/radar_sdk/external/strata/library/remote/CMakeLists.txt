
set(PROTOCOL_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/RemoteProtocolAtr22.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/RemoteProtocolAvian.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/RemoteProtocolLtr11.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/RemoteModuleRadar.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/RemotePinsAvian.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/RemotePinsLtr11.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/RemoteProcessingRadar.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/RemoteRegisters.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/RemoteRadar.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/RemoteRadarAvian.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/RemoteRadarAtr22.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/RemoteRadarLtr11.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/RemoteVendorCommands.hpp"
    )

set(PROTOCOL_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/RemoteProtocolAtr22.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/RemoteProtocolAvian.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/RemoteProtocolLtr11.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/RemoteModuleRadar.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/RemotePinsAvian.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/RemotePinsLtr11.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/RemoteProcessingRadar.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/RemoteRegisters.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/RemoteRadar.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/RemoteRadarAvian.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/RemoteRadarAtr22.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/RemoteRadarLtr11.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/RemoteVendorCommands.cpp"
    )

add_library(remote OBJECT ${PROTOCOL_HEADERS} ${PROTOCOL_SOURCES})


set(STRATA_LIBRARY_OBJECTS
    ${STRATA_LIBRARY_OBJECTS}
    $<TARGET_OBJECTS:remote>
    CACHE INTERNAL ""
    )
set(STRATA_LIBRARY_DEPENDENCIES
    ${STRATA_LIBRARY_DEPENDENCIES}
    CACHE INTERNAL ""
    )
