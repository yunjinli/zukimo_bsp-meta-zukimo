DESCRIPTION = "Radar Gesture Detection Cpp"
LICENSE = "CLOSED"
SRC_URI = "file://cpp_inference"

S = "${WORKDIR}/cpp_inference"

inherit cmake
# INHERIT += "noexec"

do_install() {
    install -d ${D}/home/root/radar_gesture_inference

    # Copy all binaries from the build output
    install -m 0755 ${B}/run_c ${D}/home/root/radar_gesture_inference/
}

FILES:${PN} += "/home/root/radar_gesture_inference /home/root/radar_gesture_inference/*" 