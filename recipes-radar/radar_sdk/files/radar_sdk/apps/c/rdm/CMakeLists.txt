add_executable(app_rdm app_rdm.c rdm_defaults.h)
target_link_libraries(app_rdm sdk_radar app_common)