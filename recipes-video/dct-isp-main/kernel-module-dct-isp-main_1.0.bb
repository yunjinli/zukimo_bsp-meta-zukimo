DESCRIPTION = "DCT ISP MAIN Linux Driver"
SECTION = "dct"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

# The inherit of module.bbclass will automatically name module packages with
# "kernel-module-" prefix as required by the oe-core build environment.
inherit module

# ZUK_REPO_* vars are defined in ../../conf/machine/include/repovars.inc
SRCREV = "${ZUK_REPO_SRCREV_ISP_MAIN}"
SRC_URI = "${ZUK_REPO_URLBASE}/${ZUK_REPO_NAME_ISP_MAIN};protocol=${ZUK_REPO_PROTOCOL};branch=main"
S = "${WORKDIR}/git/linux"

# The kernel makefile needs to contain an entry to pick up the header
# files of the kernel moduel we depend on (ccflags-y += $(EXTRA_INC)).
EXTRA_OEMAKE += "O=${STAGING_KERNEL_BUILDDIR} EXTRA_INC=-I../kernel-build-artifacts/include/dct"

# Modprobe checks the symbols (Module.symvers) for every dependency
# so there is no need to fiddle around with KBUILD_EXTRA_SYMBOLS.
DEPENDS += "kernel-module-dct-isp-dma"
DEPENDS += "kernel-module-dct-isp-pre"
RDEPENDS:${PN} += "kernel-module-dct-isp-dma"

RPROVIDES:${PN} += "kernel-module-dct-isp-main-v4l2"

# Automatically load module during boot (name of .ko file)
KERNEL_MODULE_AUTOLOAD:append = "dct_isp_main_v4l2"

# Assure to get access to the kernel headers
do_configure[depends] += "virtual/kernel:do_shared_workdir"

do_install:append() {
    install -d ${D}${includedir}/
    install -d ${D}${includedir}/dct/
    install -m 644 ${S}/isp_main_v4l2_fmt.h ${D}${includedir}/dct/isp_main_v4l2_fmt.h
    install -m 644 ${S}/../common/include/rpp_config.h ${D}${includedir}/dct/rpp_config.h
    install -m 644 ${S}/../common/include/rpp_helper.h ${D}${includedir}/dct/rpp_helper.h
    install -m 644 ${S}/../common/include/rpp_hdr_types.h ${D}${includedir}/dct/rpp_hdr_types.h
    install -m 644 ${S}/../common/include/rpp_awb_meas_drv.h ${D}${includedir}/dct/rpp_awb_meas_drv.h
    install -m 644 ${S}/../common/include/rpp_ccor_drv.h ${D}${includedir}/dct/rpp_ccor_drv.h
    install -m 644 ${S}/../common/include/rpp_ltm_drv.h ${D}${includedir}/dct/rpp_ltm_drv.h
    install -m 644 ${S}/../common/regs/rpp/rpp_csm_def.h ${D}${includedir}/dct/rpp_csm_def.h
}

# Ignore QA warning that we use the buildpath
INSANE_SKIP:${PN} += "buildpaths"
