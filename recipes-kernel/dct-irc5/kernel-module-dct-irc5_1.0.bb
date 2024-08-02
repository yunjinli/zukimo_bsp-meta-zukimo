DESCRIPTION = "DCT IRC5 Interrupt Controller Linux Driver"
SECTION = "dct"
DEPENDS = ""
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

# The inherit of module.bbclass will automatically name module packages with
# "kernel-module-" prefix as required by the oe-core build environment.
inherit module

# ZUK_REPO_* vars are defined in ../../conf/machine/include/repovars.inc
SRCREV = "${ZUK_REPO_SRCREV_IRC5}"
SRC_URI = "${ZUK_REPO_URLBASE}/${ZUK_REPO_NAME_IRC5};protocol=${ZUK_REPO_PROTOCOL};branch=main"

S = "${WORKDIR}/git/linux"

EXTRA_OEMAKE += "O=${STAGING_KERNEL_BUILDDIR}"

RPROVIDES:${PN} += "kernel-module-dct-irc5"
KERNEL_MODULE_AUTOLOAD += "dct_irc5"
