DESCRIPTION = "DCT ISP DMA Linux Driver"
SECTION = "dct"
DEPENDS = ""
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

# The inherit of module.bbclass will automatically name module packages with
# "kernel-module-" prefix as required by the oe-core build environment.
inherit module

# ZUK_REPO_* vars are defined in ../../conf/machine/include/repovars.inc
SRCREV = "${ZUK_REPO_SRCREV_ISP_DMA}"
SRC_URI = "${ZUK_REPO_URLBASE}/${ZUK_REPO_NAME_ISP_DMA};protocol=${ZUK_REPO_PROTOCOL};branch=${ZUK_REPO_BRANCH_ISP_DMA}"
S = "${WORKDIR}/git/linux"

EXTRA_OEMAKE += "O=${STAGING_KERNEL_BUILDDIR}"

# Install headers into kernel staging area to make them available
# for other out of tree build kernel modules.
do_install:append() {
    install -d ${STAGING_KERNEL_BUILDDIR}/include/dct
    install -m 644 ${S}/isp-dma.h ${STAGING_KERNEL_BUILDDIR}/include/dct/
    install -m 644 ${S}/isp-dma-v4l2.h ${STAGING_KERNEL_BUILDDIR}/include/dct/
}

DEPENDS += "kernel-module-dct-irc5"
RDEPENDS:${PN} += "kernel-module-dct-irc5"

FILES:${PN} += "${STAGING_KERNEL_BUILDDIR}/include/dct"

EXTRA_OEMAKE += "O=${STAGING_KERNEL_BUILDDIR}"

RPROVIDES:${PN} += "kernel-module-dct-isp-dma"

# Automatically load module during boot (name of .ko file)
KERNEL_MODULE_AUTOLOAD += "dct_isp_dma"

# Assure to get access to the kernel headers
do_configure[depends] += "virtual/kernel:do_shared_workdir"
# Assure to always install the files as it turned out that clean builds
# on the jenkins can cause trouble.
do_install[nostamp] = "1"
