SUMMARY = "Development packages for the zukimo device"
LICENSE = "MIT"

PR = "r10"

PACKAGE_ARCH = "${TUNE_PKGARCH}"

inherit packagegroup

PACKAGES = "                \
    packagegroup-zukimo-net \
"

RDEPENDS:packagegroup-zukimo-net = "   \
    iperf3                  \
    iproute2                \
    iproute2-tc             \
    iptables                \
    linuxptp                \
    linuxptp-configs        \
    net-tools               \
    pps-tools               \
    tcpdump                 \
"

