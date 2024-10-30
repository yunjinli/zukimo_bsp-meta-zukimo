DESCRIPTION = "DCT MVDU Linux Driver"
SECTION = "dct"
DEPENDS = ""
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

inherit module

# ZUK_REPO_* vars are defined in ../../conf/machine/include/repovars.inc
SRCREV = "${ZUK_REPO_SRCREV_MVDU}"
SRC_URI = "${ZUK_REPO_URLBASE}/${ZUK_REPO_NAME_MVDU};protocol=${ZUK_REPO_PROTOCOL};branch=${ZUK_REPO_BRANCH_MVDU}"

S = "${WORKDIR}/git/linux"

EXTRA_OEMAKE += "O=${STAGING_KERNEL_BUILDDIR}"

RDEPENDS:${PN} += "kernel-module-dct-i2c kernel-module-dct-ipc-clk"

RPROVIDES:${PN} += "kernel-module-dct-mvdu"

# Ignore QA warning that we use the buildpath
INSANE_SKIP:${PN} += "buildpaths"
