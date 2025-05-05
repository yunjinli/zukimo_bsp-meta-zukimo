set(APP_COMMON_SOURCES
    app_common.c
    json.cpp
    time_formatter.c
    util.c)

set(APP_COMMON_HEADERS
    app_common.h
    json.h
    json.hpp
    time_formatter.h
    util.h)

add_library(app_common STATIC ${APP_COMMON_SOURCES} ${APP_COMMON_HEADERS})
target_include_directories(app_common PUBLIC .)
target_link_libraries(app_common PUBLIC sdk_avian)
target_link_libraries(app_common PRIVATE sdk_radar argparse nlohmann_json)