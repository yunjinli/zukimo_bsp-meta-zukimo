SUMMARY = "boot packages for the zukimo device"
LICENSE = "MIT"

PR = "r10"

inherit packagegroup

# mandatory system util packages
# RDEPENDS:${PN} += "             \
# "

# kernel modules
RDEPENDS:${PN} += "                 \
    kernel-module-dct-gpio          \
    kernel-module-dct-ipc-gpio      \
    kernel-module-dct-ipc           \
    kernel-module-dct-ipc-clk       \
    kernel-module-dct-qspi          \
    kernel-module-dct-sdhci-cadence \
    kernel-module-fixed             \
    kernel-module-sdhci             \
    kernel-module-sdhci-pltfm       \
"
