# Simple initramfs image. Mostly used for live images.
SUMMARY = "Small image capable of booting a device."
DESCRIPTION = "Small image capable of booting a device. The kernel includes \
the Minimal RAM-based Initial Root Filesystem (initramfs), which finds the \
first 'init' program more efficiently."
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/COPYING.MIT;md5=3da9cfbcb788c80a0384361b4de20420"

INITRAMFS_SCRIPTS ?= "\
                      initramfs-framework-base \
                      initramfs-module-setup-live \
                      initramfs-module-udev \
                      initramfs-module-install \
                      initramfs-module-install-efi \
                     "

PACKAGE_INSTALL = "${INITRAMFS_SCRIPTS} ${VIRTUAL-RUNTIME_base-utils} udev base-passwd ${ROOTFS_BOOTSTRAP_INSTALL}"

# Do not pollute the initrd image with rootfs features
IMAGE_FEATURES = ""

# Don't allow the initramfs to contain a kernel
PACKAGE_EXCLUDE = "kernel-image-*"

IMAGE_NAME_SUFFIX ?= ""
IMAGE_LINGUAS = ""

IMAGE_FSTYPES = "${INITRAMFS_FSTYPES}"
inherit core-image

IMAGE_ROOTFS_SIZE = "8192"
IMAGE_ROOTFS_EXTRA_SPACE = "0"

# Use the same restriction as initramfs-module-install
COMPATIBLE_HOST = '(x86_64.*|i.86.*|arm.*|aarch64.*|loongarch64.*)-(linux.*|freebsd.*)'


SUMMARY = "initramfs-framework module for networking"

FILESEXTRAPATHS:prepend := "${THISDIR}:${THISDIR}/files:"

INITRAMFS_FSTYPES = "cpio.xz cpio.xz.u-boot"
INITRAMFS_MAXSIZE = "571361"
INITRAMFS_SCRIPTS = "\
					  initramfs-module-udev       \
					 "

include zukimo-image-common.inc

PACKAGE_INSTALL = "           \
	${INITRAMFS_SCRIPTS}            \
	${VIRTUAL-RUNTIME_base-utils}   \
	udev base-passwd                \
	packagegroup-zukimo-boot        \
	custom-initramfs                \
"

PACKAGE_EXCLUDE = "           \
	grub                            \
	grub-efi                        \
	parted                          \
"