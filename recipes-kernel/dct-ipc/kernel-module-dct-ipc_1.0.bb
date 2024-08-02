DESCRIPTION = "DCT IPC Linux Drivers"
SECTION = "dct"
DEPENDS = ""
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

inherit module

# ZUK_REPO_* vars are defined in ../../conf/machine/include/repovars.inc
SRCREV = "${ZUK_REPO_SRCREV_IPC}"
SRC_URI = "${ZUK_REPO_URLBASE}/${ZUK_REPO_NAME_IPC};protocol=${ZUK_REPO_PROTOCOL};branch=main"

S = "${WORKDIR}/git/linux"

EXTRA_OEMAKE += "O=${STAGING_KERNEL_BUILDDIR}"

RPROVIDES:${PN} += "kernel-module-dct-ipc"

do_install:append() {
    install -d ${STAGING_KERNEL_BUILDDIR}/include/dct
    install -m 644 ${S}/dct_ipc.h ${STAGING_KERNEL_BUILDDIR}/include/dct/dct_ipc.h
    install -m 644 ${S}/dct_ipc_fsp.h ${STAGING_KERNEL_BUILDDIR}/include/dct/dct_ipc_fsp.h
}

FILES:${PN} += "${STAGING_KERNEL_BUILDDIR}/include/dct"

# Assure to get access to the kernel headers
do_configure[depends] += "virtual/kernel:do_shared_workdir"
# Assure to always install the files as it turned out that clean builds
# on the jenkins can cause trouble.
do_install[nostamp] = "1"
