SUMMARY = "U-boot boot script"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

DEPENDS = "u-boot-mkimage-native"

INHIBIT_DEFAULT_DEPS = "1"

SRC_URI = "file://boot.cmd.in"

do_compile() {
    sed -e 's/@@DEVICETREE_BASENAME@@/${KERNEL_DEVICETREE_BASENAME}/' \
        -e 's/@@KERNEL_IMAGETYPE@@/${KERNEL_IMAGETYPE}-${MACHINE}.bin/' \
        "${WORKDIR}/boot.cmd.in" > "${WORKDIR}/boot.cmd"
    mkimage -A ${UBOOT_ARCH} -T script -C none -n "Boot script" -d "${WORKDIR}/boot.cmd" boot.scr
}

inherit kernel-arch deploy nopackages

do_deploy() {
    install -d ${DEPLOYDIR}
    install -m 0644 boot.scr ${DEPLOYDIR}
}

addtask do_deploy after do_compile before do_build

PROVIDES += "u-boot-default-script"
