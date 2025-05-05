set(RAPIDJSON_DIR "${CMAKE_CURRENT_SOURCE_DIR}/rapidjson/include")

add_interface_library(rapidjson)
interface_target_include_directories(rapidjson ${RAPIDJSON_DIR})
interface_target_compile_definitions(rapidjson -DRAPIDJSON_HAS_STDSTRING)
