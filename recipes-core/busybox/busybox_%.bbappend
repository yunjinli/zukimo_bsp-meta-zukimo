# Copyright (c) 2022, Arm Limited.
#
# SPDX-License-Identifier: MIT

FILESEXTRAPATHS:prepend := "${@bb.utils.contains('DISTRO_FEATURES', \
                                                 'refstack-tests', \
                                                 '${THISDIR}/files:', \
                                                 '', \
                                                 d)}"

# As Bats requires the nl utility, configure CONFIG_NL=y for busybox if we are
# using Bats
SRC_URI:append := "${@bb.utils.contains('DISTRO_FEATURES', \
                                        'refstack-tests', \
                                        'file://nl.cfg', \
                                        '', \
                                        d)}"
