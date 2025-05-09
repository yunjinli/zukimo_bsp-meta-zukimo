SUMMARY = "ZUKIMO additional udev rules"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

S = "${WORKDIR}"

SRC_URI = " \
    file://video-dev.rules \
"

do_install:append() {
	install -d ${D}${sysconfdir}/udev/rules.d
	install -m 0644 ${S}/video-dev.rules ${D}${sysconfdir}/udev/rules.d/
}

FILES:${PN} +=  "       \
    /etc/udev/rules.d/  \
"
