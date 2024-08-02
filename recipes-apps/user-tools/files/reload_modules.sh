#!/bin/sh

rmmod dct_isp_pre_v4l2
rmmod dct_isp_main_v4l2
rmmod dct_isp_dma

devmem2 0x3800008 w 0x1F807B0D
usleep 200000
devmem2 0x3800008 w 0x1F807BED

modprobe dct_isp_pre_v4l2
modprobe dct_isp_main_v4l2
