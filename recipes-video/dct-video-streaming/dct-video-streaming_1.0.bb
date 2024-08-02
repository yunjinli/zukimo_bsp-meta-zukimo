DESCRIPTION = "DCT video streaming"
SECTION = "dct"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

SRC_URI = "file://streaming.py"

S = "${WORKDIR}"

RDEPENDS:${PN} = "python3-pip python3-socketio python3-logging"

do_configure () {
	:
}

do_compile() {
	:
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 ${S}/streaming.py ${D}${bindir}
}
