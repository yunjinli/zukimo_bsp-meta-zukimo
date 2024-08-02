INITRAMFS_FSTYPES = "cpio.xz cpio.xz.u-boot"

INITRAMFS_MAXSIZE = "580000"

include zukimo-image-common.inc

PACKAGE_INSTALL:append = "          \
    packagegroup-zukimo-core        \
    packagegroup-zukimo-dev-base    \
    dropbear             \
"

IMAGE_FEATURES:remove = "   \
    openssh-sftp-server     \
"
IMAGE_FEATURES:append = "   \
    allow-empty-password    \
    allow-root-login        \
    ssh-server-dropbear     \
"
