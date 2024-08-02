SUMMARY = "Place a git revision file in the sysroot"
LICENSE = "MIT"

inherit image-buildinfo

REVISION_INFO_FILE = "build-layers-git-revisions"
S = "${WORKDIR}"



python do_configure() {
    with open(d.expand("${S}/${REVISION_INFO_FILE}"), "w") as file:
        file.writelines(get_layer_revs(d))
}

do_install() {
    install -d ${D}${sysconfdir}
    install -m 0644 ${S}/${REVISION_INFO_FILE} ${D}${sysconfdir}
}

# Force configure & install task to be executed always.
# It turns out that for the jenkins build ROOTFS_POSTPROCESS_COMMAND is not getting executed
# when software was not changed.
do_configure[nostamp] = "1"
do_install[nostamp] = "1"

FILES_${PN} += "${sysconfdir}"
