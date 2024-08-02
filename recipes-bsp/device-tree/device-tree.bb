DESCRIPTION = "Build the devicetree independent from kernel"
SECTION = "dtb"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

COMPATIBLE_MACHINE:zukimo       = "zukimo"
COMPATIBLE_MACHINE:zukimo_haps  = "zukimo_haps"

inherit devicetree

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"
 
SRC_URI:append = " \
    file://zukimo.dts \
    file://zukimo_haps.dts \
    file://zukimo_pcie_rc.dts \
    file://zukimo_pcie_ep.dts \
    file://zukimo_common.dtsi \
    file://zukimo_fpdlink0.dtsi \
    file://zukimo_fpdlink1.dtsi \
    file://zukimo_fpdlink2.dtsi \
    file://zukimo_fpdlink3.dtsi \
    file://zukimo_camera01.dtsi \
    file://zukimo_camera23.dtsi \
    file://zukimo_camera45.dtsi \
    file://zukimo_camera67.dtsi \
    file://zukimo_isp0.dtsi \
    file://zukimo_isp1.dtsi \
    file://zukimo_pcie_rc.dtsi \
    file://zukimo_pcie_ep.dtsi \
"

KERNEL_DEVICETREE:zukimo = "zukimo.dtb"
KERNEL_DEVICETREE:zukimo_haps = "zukimo_haps.dtb"
KERNEL_DEVICETREE:zukimo_pcie_rc = "zukimo_pcie_rc.dtb"
KERNEL_DEVICETREE:zukimo_pcie_ep = "zukimo_pcie_ep.dtb"
