# Need the following for boot-wrapper-aarch64 only:

FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"
FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

PATCHTOOL = "git"

SRCREV:zukimo = "b08ffdffdce95e267e782366f4a77bf6b5537b28"
LIC_FILES_CHKSUM:zukimo = "file://Licenses/README;md5=2ca5f2c35c8cc335f0a19756634782f1"

# We upgraded u-boot for zukimo stop applying patches from other layers that are not needed any more:
SRC_URI:remove:zukimo = " \
    file://0001-riscv32-Use-double-float-ABI-for-rv32.patch \
    file://0001-riscv-fix-build-with-binutils-2.38.patch \
    file://0001-i2c-fix-stack-buffer-overflow-vulnerability-in-i2c-m.patch \
    file://0001-fs-squashfs-sqfs_read-Prevent-arbitrary-code-executi.patch \
    file://0001-net-Check-for-the-minimum-IP-fragmented-datagram-siz.patch \
    file://0001-fs-squashfs-Use-kcalloc-when-relevant.patch \
    "

SRC_URI:append:zukimo = " \
    file://0001-zukimo-add-basic-support-for-zukimo-base-board.patch \
    file://0002-zukimo-add-psci-services-to-u-boot-untested.patch \
    file://0003-zukimo-power-up-gicr-before-initializing.patch \
    file://0004-zukimo-add-devicetree.patch \
    file://0005-zukimo-debug-startup.patch \
    file://0006-zukimo-armv8-Enable-icache-when-switching-exception-.patch \
    file://0007-zukimo-fix-board_f.patch \
    file://0008-zukimo-add-doorbell-psci-and-sdhc-integration.patch \
    file://0009-zukimo-add-gem-support.patch \
	file://0010-zukimo-add-dct-sdhci-cadence.patch \
	file://0011-Add-zukimo-reset-controller-support.patch \
	file://0012-initialize-default-tcp-handler.patch \
"

SRC_URI:append:zukimo = " \
    file://bootargs.cfg \
	file://zukimo.h \
    file://zukimo_haps.h \
    file://zukimo_defconfig \
    file://zukimo_haps_defconfig \
"

# add the zukimo configuration file to u-boot
do_configure:prepend () {
	install ${WORKDIR}/zukimo.h ${S}/include/configs/
	install ${WORKDIR}/zukimo_haps.h ${S}/include/configs/
	install ${WORKDIR}/zukimo_defconfig ${S}/configs/
	install ${WORKDIR}/zukimo_haps_defconfig ${S}/configs/
}
