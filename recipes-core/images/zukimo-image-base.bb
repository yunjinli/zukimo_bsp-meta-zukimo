DESCRIPTION = "Base image for the zukimo"
# Based on core-image-minimal

IMAGE_FSTYPES = "tar.gz squashfs"

inherit core-image
inherit populate_sdk_base

include zukimo-image-common.inc

IMAGE_BASENAME = "zukimo-image-base"

INIT_MANAGER = "systemd"

DEFAULT_TIMEZONE = "Europe/Berlin"

IMAGE_INSTALL:append = "            \
    gstreamer1.0-plugins-good       \
    packagegroup-core-full-cmdline  \
    packagegroup-zukimo-core        \
    packagegroup-zukimo-dev-base    \
    packagegroup-zukimo-dev-ext     \
    packagegroup-zukimo-system      \
    packagegroup-zukimo-net         \
    gen-revision-file               \
"

# Disable nfs server, when needed add kernel config for nfs fs
IMAGE_FEATURES:remove = "   \
    openssh-sftp-server     \
"

IMAGE_FEATURES:append = "   \
    allow-empty-password    \
    allow-root-login        \
    read-only-rootfs        \
    empty-root-password     \
    serial-autologin-root   \
    ssh-server-dropbear     \
    tools-sdk               \
"

# Assure that in SDK build sdk cmake is getting used
TOOLCHAIN_HOST_TASK:append = " \
    nativesdk-cmake            \
    nativesdk-protobuf         \
    nativesdk-protobuf-compiler\
"

# Add target compiled libraries to the sdk
TOOLCHAIN_TARGET_TASK:append = "  \
   protobuf-dev                   \
   libstdc++-staticdev            \
   kernel-devsrc                  \
   kernel-module-dct-isp-main-dev \
"

# We have to overwrite the SDKIMAGE_FEATURES variable because we
# don't want the debug symbols installed for every package.
SDKIMAGE_FEATURES:remove = "dbg-pkgs"

# normaly there should be no need to set this manually
# as the feature is defined by 'empty-root-password' and
# 'serial-autologin-root' need to check why we have to do.
ROOTFS_POSTPROCESS_COMMAND += "serial_autologin_root"
