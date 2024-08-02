SUMMARY = "initramfs-framework module for networking"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/COPYING.MIT;md5=3da9cfbcb788c80a0384361b4de20420"

FILESEXTRAPATHS:prepend := "${THISDIR}:${THISDIR}/files:"

SRC_URI = " \
	file://bootfs \
	file://nfsrootfs \
	file://squashfs \
"

S = "${WORKDIR}"

do_install:append () {
	install -d ${D}/init.d
	install -m 0755 ${WORKDIR}/bootfs ${D}/init.d/86-bootfs
	install -m 0755 ${WORKDIR}/nfsrootfs ${D}/init.d/85-nfsrootfs
	install -m 0755 ${WORKDIR}/squashfs ${D}/init.d/87-squashfs
}

FILES:${PN} = "\
	/init.d/85-nfsrootfs \
	/init.d/86-bootfs \
	/init.d/87-squashfs \
"
