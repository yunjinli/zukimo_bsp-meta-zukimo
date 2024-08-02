add_to_rootfs_image() {
    # write a timestamp file...
    bbnote "creating timestamp file..."
    date +"%Y-%m-%d %H:%M:%S" >${IMAGE_ROOTFS}/etc/timestamp

    # write a version file...
    bbnote "creating image version file..."
    echo "${RELEASE_VERSION_HPC_SYSTEM}" >${IMAGE_ROOTFS}/etc/firmware.ver
    echo "${RELEASE_GITHASH_HPC_SYSTEM}" >>${IMAGE_ROOTFS}/etc/firmware.ver
}
