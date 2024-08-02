SUMMARY = "Development packages for the zukimo device"
LICENSE = "MIT"

PR = "r10"

PACKAGE_ARCH = "${TUNE_PKGARCH}"

inherit packagegroup

PACKAGES = "                        \
    packagegroup-zukimo-dev-base    \
    packagegroup-zukimo-dev-ext     \
"

# tools needed for the board bringup
RDEPENDS:packagegroup-zukimo-dev-base = "   \
    devmem2                 \
    dosfstools              \
    graphviz                \
    gst-shark               \
    htop                    \
    i2c-tools               \
    kernel-module-dmatest   \
    libgpiod-tools          \
    lrzsz                   \
    pciutils                \
    perf                    \
    stress-ng               \
    sysbench                \
    tree                    \
    user-tools              \
"

# Does not compile when some DISTRO_FEATURES are removed.
# If needed debug which dependency is missing that was pulled
# in from one of the following distro features:
# "alsa bluetooth pcmcia wifi 3g x11 nfs"
# RDEPENDS:packagegroup-zukimo-dev-gst = "    \
#     gstd                                    \
#     gst-instruments                         \
#     gst-instruments-dev                     \
# "

# add common debugging tools
RDEPENDS:packagegroup-zukimo-dev-ext = "   \
    gdb                 \
    gdbserver           \
    ldd                 \
    strace              \
"
