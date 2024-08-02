FILESEXTRAPATHS:prepend := "${THISDIR}:${THISDIR}/files:"

#
# Zukimo Machine
#
COMPATIBLE_MACHINE:zukimo = "zukimo"
KMACHINE:zukimo = "zukimo"

SRC_URI:append:zukimo = " file://zukimo-kmeta;type=kmeta;name=zukimo-kmeta;destsuffix=zukimo-kmeta"
SRC_URI:append:zukimo = " \
    file://0001-zukimo-add-arch.patch \
    file://0003-zukimo-add-dct-uart.patch \
    file://0004-zukimo-add-quad-flash-support.patch \
    file://0005-adv7511-add-of-support.patch \
    file://0006-zukimo-add-dct-irc5.patch \
    file://0007-Apply-v16-v4l-routing-and-streams-support.patch \
    file://0008-Apply-v15-i2c-atr-and-FPDLink.patch \
    file://0009-media-i2c-ds90ub960-Add-support-for-DS90UB954-Q1.patch \
    file://0010-media-subdev-Split-V4L2_SUBDEV_ROUTING_NO_STREAM_MIX.patch \
    file://0011-Integrate-imx390-driver-from-ti-kernel.patch \
    file://0012-Enable-debug-for-fpdlink-and-imx390-drivers.patch \
    file://0013-reset-Add-generic-GPIO-reset-driver.patch \
    file://0014-media-i2c-ds90ub9x3-Fix-sub-device-matching.patch \
    file://0015-media-i2c-ds90ub960-Configure-CSI-2-continuous-clock.patch \
    file://0016-media-i2c-ds90ub953-Use-v4l2_fwnode_endpoint_parse.patch \
    file://0017-media-i2c-ds90ub913-Use-v4l2_fwnode_endpoint_parse.patch \
    file://0018-media-i2c-ds90ub953-Handle-V4L2_MBUS_CSI2_NONCONTINU.patch \
    file://0019-media-i2c-ds90ub960-Allow-FPD-Link-async-mode.patch \
    file://0020-media-i2c-ds90ub953-Restructure-clkout-management.patch \
    file://0021-media-i2c-ds90ub953-Support-non-sync-mode.patch \
    file://0022-media-i2c-ds90ub960-Rename-RXPORT_MODE_CSI2_ASYNC-to.patch \
    file://0023-macb-gem-fixes.patch \
    file://0024-media-i2c-ds90ub9xx-imx390-Disable-debug-loglevel.patch \
    file://0025-adv7511-add-support-for-embedded-sync.patch \
    file://0026-IMX390-replace-wdr-register-map.patch \
    file://0027-Workaround-for-multiple-instantiation-of-ds90ub954.patch \
    file://0028-macb-gem-Enable-ptp-capability.patch \
    file://0029-Zukimo-add-reset-controller-support-to-clk_init.patch \
    file://0030-Add-support-for-DCT-Zukimo-reset-controller.patch \
    file://0031-Add-bindings-for-Zukimo-clocks.patch \
    file://0032-Implement-test-pattern-ctrl-in-ds90ub960.patch \
    file://0033-Fix-ds90ub960-tpg.patch \
    file://0034-Add-tpg-fps-sysfs.patch \
"

# The device tree gets selected in the device-tree recipe that builds
# the dt out of kernel tree
KERNEL_DEVICETREE = ""
