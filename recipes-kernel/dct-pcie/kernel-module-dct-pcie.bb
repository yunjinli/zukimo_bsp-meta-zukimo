DESCRIPTION = "PCIe Root and Endpoint Linux driver"
SECTION = "dct"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

# The inherit of module.bbclass will automatically name module packages with
# "kernel-module-" prefix as required by the oe-core build environment.
inherit module

# ZUK_REPO_* vars are defined in ../../conf/machine/include/repovars.inc
SRCREV = "${ZUK_REPO_SRCREV_PCIE}"
SRC_URI = "${ZUK_REPO_URLBASE}/${ZUK_REPO_NAME_PCIE};protocol=${ZUK_REPO_PROTOCOL};branch=${ZUK_REPO_BRANCH_PCIE}"
S = "${WORKDIR}/git/linux"

# The kernel makefile needs to contain an entry to pick up the header
# files of the kernel moduel we depend on (ccflags-y += $(EXTRA_INC)).
EXTRA_OEMAKE += "O=${STAGING_KERNEL_BUILDDIR} EXTRA_INC=-I${STAGING_KERNEL_BUILDDIR}/include/dct"

# Modprobe checks the symbols (Module.symvers) for every dependency
# so there is no need to fiddle around with KBUILD_EXTRA_SYMBOLS.
DEPENDS += "kernel-module-dct-gpio"
RDEPENDS:${PN} += "kernel-module-dct-gpio"

RPROVIDES:${PN} += "kernel-module-dct-pcie"

# Automatically load module during boot (name of .ko file)
KERNEL_MODULE_AUTOLOAD:append = "dct_pcie"

# Assure to get access to the kernel headers
do_configure[depends] += "virtual/kernel:do_shared_workdir"
