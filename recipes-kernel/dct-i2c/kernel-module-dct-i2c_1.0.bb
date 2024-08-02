DESCRIPTION = "DCT I2C Linux Drivers"
SECTION = "dct"
DEPENDS = ""
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

inherit module

# ZUK_REPO_* vars are defined in ../../conf/machine/include/repovars.inc
SRCREV = "${ZUK_REPO_SRCREV_I2C}"
SRC_URI = "${ZUK_REPO_URLBASE}/${ZUK_REPO_NAME_I2C};protocol=${ZUK_REPO_PROTOCOL};branch=main"
S = "${WORKDIR}/git/linux"

EXTRA_OEMAKE += "O=${STAGING_KERNEL_BUILDDIR}"

RPROVIDES:${PN} += "kernel-module-dct-i2c"
KERNEL_MODULE_AUTOLOAD += "dct_i2c"
