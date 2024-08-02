DESCRIPTION = "DCT IPC Clock Driver"
SECTION = "dct"
DEPENDS = "kernel-module-dct-ipc"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

inherit module

# ZUK_REPO_* vars are defined in ../../conf/machine/include/repovars.inc
SRCREV = "${ZUK_REPO_SRCREV_IPC_CLK}"
SRC_URI = "${ZUK_REPO_URLBASE}/${ZUK_REPO_NAME_IPC_CLK};protocol=${ZUK_REPO_PROTOCOL};branch=main"
S = "${WORKDIR}/git/linux"

EXTRA_OEMAKE += "O=${STAGING_KERNEL_BUILDDIR} EXTRA_INC=-I${STAGING_KERNEL_BUILDDIR}/include/dct"

RPROVIDES:${PN} += "kernel-module-dct-ipc-clk"

# Assure to get access to the kernel headers
do_configure[depends] += "virtual/kernel:do_shared_workdir"
