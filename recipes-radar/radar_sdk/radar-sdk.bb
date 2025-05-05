DESCRIPTION = "Radar SDK built with CMake"
LICENSE = "CLOSED"
SRC_URI = "file://radar_sdk"

S = "${WORKDIR}/radar_sdk"

inherit cmake

do_install() {
    install -d ${D}/home/root/radar_app

    # Copy all binaries from the build output
    install -m 0755 ${B}/bin/* ${D}/home/root/radar_app/
}

FILES:${PN} += "/home/root/radar_app /home/root/radar_app/*"
