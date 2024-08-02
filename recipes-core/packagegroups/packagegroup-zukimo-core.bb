SUMMARY = "Core packages for the zukimo device"
LICENSE = "MIT"

PR = "r10"

inherit packagegroup

# mandatory system util packages
RDEPENDS:${PN} += "                 \
    camera-tools                    \
    dct-nna-firmware                \
    gstreamer1.0                    \
    gstreamer1.0-plugins-bad-bayer  \
    gstreamer1.0-plugins-base       \
    gstreamer1.0-plugins-good       \
    tzdata                          \
    udev-rules                      \
    v4l-utils                       \
    googletest                      \
    system-test                     \
"

# kernel modules
RDEPENDS:${PN} += "                 \
    kernel-module-dct-cpufreq       \
    kernel-module-dct-gpio          \
    kernel-module-dct-i2c           \
    kernel-module-dct-ipc           \
    kernel-module-dct-ipc-clk       \
    kernel-module-dct-ipc-gpio      \
    kernel-module-dct-irc5          \
    kernel-module-dct-isp-dma       \
    kernel-module-dct-isp-main      \
    kernel-module-dct-isp-pre       \
    kernel-module-dct-mvdu          \
    kernel-module-dct-nna           \
    kernel-module-dct-pcie          \
    kernel-module-dct-qspi          \
    kernel-module-dct-sdhci-cadence \
    kernel-module-fixed             \
    kernel-module-sdhci             \
    kernel-module-sdhci-pltfm       \
"
