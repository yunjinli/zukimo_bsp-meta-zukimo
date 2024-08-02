SUMMARY = "ZUKIMO Helper Scripts and Files"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "\
	file://reload_modules.sh \
	file://test_video_pipeline.sh \
	file://.profile \
"

S = "${WORKDIR}"

do_install:append () {
	install -d ${D}/home/root

	install -m 755 ${S}/reload_modules.sh   	${D}/home/root
	install -m 755 ${S}/test_video_pipeline.sh  ${D}/home/root
	install -m 755 ${S}/.profile           		${D}/home/root
}

FILES:${PN} +=  "   \
    /home           \
    /home/*         \
"

RDEPENDS:${PN}:append = "bash"
