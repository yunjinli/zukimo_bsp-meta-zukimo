
set(MODULES_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/interfaces/IModule.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/interfaces/IModuleRadar.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/interfaces/IModuleMeasurementTimer.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/radar/ModuleRadar.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/radar/ModuleRadarSingle.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/radar/ModuleRadarCascaded.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ModuleMeasurementTimer.hpp"
    )

set(MODULES_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/radar/ModuleRadar.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/radar/ModuleRadarSingle.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/radar/ModuleRadarCascaded.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ModuleMeasurementTimer.cpp"
    )

add_library(modules OBJECT ${MODULES_HEADERS} ${MODULES_SOURCES})


set(STRATA_LIBRARY_OBJECTS
    ${STRATA_LIBRARY_OBJECTS}
    $<TARGET_OBJECTS:modules>
    CACHE INTERNAL ""
    )
set(STRATA_LIBRARY_DEPENDENCIES
    ${STRATA_LIBRARY_DEPENDENCIES}
    CACHE INTERNAL ""
    )
