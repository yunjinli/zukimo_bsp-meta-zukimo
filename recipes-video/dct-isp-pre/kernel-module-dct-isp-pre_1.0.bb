DESCRIPTION = "DCT ISP PRE Linux Driver"
SECTION = "dct"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

# The inherit of module.bbclass will automatically name module packages with
# "kernel-module-" prefix as required by the oe-core build environment.
inherit module

# ZUK_REPO_* vars are defined in ../../conf/machine/include/repovars.inc
SRCREV = "${ZUK_REPO_SRCREV_ISP_PRE}"
SRC_URI = "${ZUK_REPO_URLBASE}/${ZUK_REPO_NAME_ISP_PRE};protocol=${ZUK_REPO_PROTOCOL};branch=main"
S = "${WORKDIR}/git/linux"

# The kernel makefile needs to contain an entry to pick up the header
# files of the kernel moduel we depend on (ccflags-y += $(EXTRA_INC)).
EXTRA_OEMAKE += "O=${STAGING_KERNEL_BUILDDIR} EXTRA_INC=-I../kernel-build-artifacts/include/dct"


# Install headers into kernel staging area to make them available
# for other out of tree build kernel modules.
do_install:append() {
    install -d ${STAGING_KERNEL_BUILDDIR}/include/dct
    install -m 644 ${S}/isp_pre_v4l2.h ${STAGING_KERNEL_BUILDDIR}/include/dct/
}

# Modprobe checks the symbols (Module.symvers) for every dependency
# so there is no need to fiddle around with KBUILD_EXTRA_SYMBOLS.
DEPENDS += "kernel-module-dct-isp-dma"
RDEPENDS:${PN} += "kernel-module-dct-isp-dma"

RPROVIDES:${PN} += "kernel-module-dct-isp-pre-v4l2"
RPROVIDES:${PN} += "kernel-module-dw-csi"
RPROVIDES:${PN} += "kernel-module-dw-dphy"

# Automatically load module during boot (name of .ko file)
KERNEL_MODULE_AUTOLOAD:append = "dct_isp_pre_v4l2"
KERNEL_MODULE_AUTOLOAD:append = "dw-csi"

# Assure to get access to the kernel headers
do_configure[depends] += "virtual/kernel:do_shared_workdir"

# Ignore QA warning that we use the buildpath
INSANE_SKIP:${PN} += "buildpaths"
