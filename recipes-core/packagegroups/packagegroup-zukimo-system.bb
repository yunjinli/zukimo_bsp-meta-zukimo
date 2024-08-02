SUMMARY = "System packages for the zukimo device"
LICENSE = "MIT"

PR = "r10"

inherit packagegroup

# optional packages for the zukimo system
RDEPENDS:${PN} += "             \
    custom-services             \
"
