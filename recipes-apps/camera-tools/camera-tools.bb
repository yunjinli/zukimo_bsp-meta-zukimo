DESCRIPTION = "DCT Camera Tools"
SECTION = "dct"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "\
	file://dct-v4l2-3a \
	file://dct-v4l2-test \
	file://imxdump \
"

S = "${WORKDIR}"

do_install:append () {
	install -d ${D}/usr/local/bin

	install -m 755 ${S}/dct-v4l2-3a    ${D}/usr/local/bin
	install -m 755 ${S}/dct-v4l2-test  ${D}/usr/local/bin
	install -m 755 ${S}/imxdump        ${D}/usr/local/bin
}

FILES:${PN} += "                 \
    /usr/local/bin/dct-v4l2-3a   \
    /usr/local/bin/dct-v4l2-test \
    /usr/local/bin/imxdump       \
"
