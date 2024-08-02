DESCRIPTION = "DCT NNA firmware"
SECTION = "dct"
DEPENDS = "kernel-module-dct-nna"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "\
    file://mwcDemo.elf \
"

S = "${WORKDIR}"

do_install:append () {
	install -d ${D}/${libdir}/firmware/dct
	install -m 755 ${S}/mwcDemo.elf  ${D}/${libdir}/firmware/dct
}

FILES:${PN} +=  "   \
    /usr/lib/firmware/dct           \
    /usr/lib/firmware/dct/*         \
"

INSANE_SKIP:${PN} += "arch"
