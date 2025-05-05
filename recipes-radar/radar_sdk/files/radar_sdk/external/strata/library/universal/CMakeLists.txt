
file(GLOB_RECURSE UNIVERSAL_HEADERS *.h)

add_custom_target(universal SOURCES ${UNIVERSAL_HEADERS})
