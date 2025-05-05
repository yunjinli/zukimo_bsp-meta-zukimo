add_executable(bgt60-configurator-cli bgt60-configurator-cli.cpp)
target_link_libraries(bgt60-configurator-cli sdk_radar sdk_avian app_common argparse)