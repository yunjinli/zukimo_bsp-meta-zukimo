/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Configuration for Zukimo platform. Parts were derived from other ARM
 *   configurations.
 * (C) Copyright 2023
 * Stefan Stürke <stefan.stuerke@dreamchip.de>
 */

#ifndef __ZUKIMO_H
#define __ZUKIMO_H

#include <linux/stringify.h>

/* Zukimo does not rely on CPU_RELEASE_ADDR but it needs to be defined for (unused) code in start.S to compile.
 * So we define it here to make the compiler happy without having to patch start.S
 */
#define CPU_RELEASE_ADDR        0

/* define a magic number to put CPU0 in the role of a secondary CPU */
#define CPU_ROLE_MAIN			0
#define CPU_ROLE_SECONDARY		0x4041424344454647

/* define states to track boot progress of cpu cores */
#define CPU_STATE_NOT_IN_UBOOT	0
#define CPU_STATE_WAIT_GICD		1
#define CPU_STATE_WAIT_GIC_IRQ	2
#define CPU_STATE_WAIT_OS_ADDR	3
#define CPU_STATE_JUMP_TO_OS	4

#define LPIO_QSPI_FIFO_BASE     0x22000000
#define LPIO_QSPI_CTRL_BASE     0x22010000
#define LPIO_I2C0_BASE          0x22020000
#define LPIO_I2C1_BASE          0x22030000
#define LPIO_I2C2_BASE          0x22040000
#define LPIO_SPI0_BASE          0x22050000
#define LPIO_SPI1_BASE          0x22060000
#define LPIO_SPI2_BASE          0x22070000
#define LPIO_UART0_BASE         0x22080000
#define LPIO_UART1_BASE         0x22090000
#define LPIO_GPIOA_BASE         0x220a0000
#define LPIO_TIMERS_BASE        0x220b0000

/* Generic Interrupt Controller Definitions */
#ifdef CONFIG_GICV3
#define GICD_BASE				0x11000000
#define GICR_BASE				0x11040000
#else
#error Zukimo uses GICV3, please select CONFIG_GICV3
#endif /* !CONFIG_GICV3 */

#if !defined(CONFIG_DM_ETH)
/* TODO: need to configure the right ethernet controller here. This one is just copied from VExpress board. */
#define CONFIG_SMC91111			1
#define CONFIG_SMC91111_BASE    0x029000000
#endif

/* Serial Configuration */
#define ZUKIMO_UART_CLOCK		25000000

/* Physical Memory Map */
#define PHYS_SRAM				0x10000000ULL	/* Internal SRAM, 512 kB */
#define PHYS_SRAM_SIZE			0x00080000ULL
#if CONFIG_NR_DRAM_BANKS == 0
#define CFG_SYS_SRAM_BASE		PHYS_SRAM
#endif

#define PHYS_SDRAM_1			0x80000000ULL	/* SDRAM Bank #1, 2GB */
#define PHYS_SDRAM_1_SIZE		0x80000000ULL
#if CONFIG_NR_DRAM_BANKS >= 1
#define CFG_SYS_SDRAM_BASE		PHYS_SDRAM_1
#endif

#if CONFIG_NR_DRAM_BANKS >= 2
#define PHYS_SDRAM_2			0x800000000ULL /* SDRAM Bank #2, 6GB */
#define PHYS_SDRAM_2_SIZE		0x180000000ULL
#endif

/* Initial environment variables */

#ifdef CONFIG_DISTRO_DEFAULTS
#define BOOTENV_DEV_MEM(devtypeu, devtypel, instance) \
	"bootcmd_mem= " \
		"source ${scriptaddr}; " \
		"if test $? -eq 1; then " \
		"  env import -t ${scriptaddr}; " \
		"  if test -n $uenvcmd; then " \
		"    echo Running uenvcmd ...; " \
		"    run uenvcmd; " \
		"  fi; " \
		"fi\0"
#define BOOTENV_DEV_NAME_MEM(devtypeu, devtypel, instance) "mem "

#define BOOT_TARGET_DEVICES(func)	\
	func(MEM, mem, na)		\
	func(PXE, pxe, na)		\
	func(DHCP, dhcp, na)

#include <config_distro_bootcmd.h>
#else
#define BOOTENV
#endif

#define ZUKIMO_KERNEL_ADDR	    0x80080000
#define ZUKIMO_PXEFILE_ADDR	    0x8fb00000
#define ZUKIMO_FDT_ADDR		    0x82000000
#define ZUKIMO_SCRIPT_ADDR	    0x8fd00000
#define ZUKIMO_RAMDISK_ADDR	    0x83000000

#define ENV_MEM_LAYOUT_SETTINGS \
	"kernel_addr_r=" __stringify(ZUKIMO_KERNEL_ADDR) "\0" \
	"pxefile_addr_r=" __stringify(ZUKIMO_PXEFILE_ADDR) "\0" \
	"fdt_addr_r=" __stringify(ZUKIMO_FDT_ADDR) "\0" \
	"fdtfile=board.dtb\0" \
	"scriptaddr=" __stringify(ZUKIMO_SCRIPT_ADDR) "\0"  \
	"ramdisk_addr_r=" __stringify(ZUKIMO_RAMDISK_ADDR) "\0" \

#define EXTRA_BOOTENV \
	"bootdelay=5\0" \
	"gateway=192.168.1.1\0" \
	"ipaddr=192.168.1.2\0" \
	"netmask=255.255.255.0\0" \
	"rootpath=/export/zukimo-rfs\0" \
	"serverip=192.168.1.1\0" \

#define BOOT_CMDS \
	"tftp_path=zukimo/\0" \
	"load_dtb_sdc=fatload mmc 0 ${fdt_addr_r} dtb.bin\0" \
	"load_kernel_sdc=fatload mmc 0 ${kernel_addr_r} image.bin\0" \
	"load_rootfs_sdc=fatload mmc 0 ${ramdisk_addr_r} rootfs.bin\0" \
	"load_dtb_tftp=tftpboot ${fdt_addr_r} ${tftp_path}dtb.bin\0" \
	"load_kernel_tftp=tftpboot ${kernel_addr_r} ${tftp_path}image.bin\0" \
	"load_rootfs_tftp=tftpboot ${ramdisk_addr_r} ${tftp_path}rootfs.bin\0" \
	"add_ip=setenv bootargs ${bootargs} ip=${ipaddr}:${serverip}:${gateway}:${netmask}:${hostname}:${ethdev}:off\0" \
	"add_nfs=setenv bootargs ${bootargs} rw root=/dev/nfs nfsroot=${serverip}:${rootpath},nfsvers=3\0" \
	"add_squashfs=setenv bootargs ${bootargs} ro vfat=/dev/mmcblk0p1:/tmp/bootfs squashfs=/tmp/bootfs/squashfs.bin:/rootfs \0" \
	"add_dbg=setenv bootargs ${bootargs} debug user_debug=31 init_debug=31 loglevel=9\0" \
	"bootcmd_sdc=echo booting from SD, be patient..." \
	"; echo load kernel; run load_kernel_sdc"\
	"; echo load rootfs; run load_rootfs_sdc" \
	"; echo load dtb; run load_dtb_sdc" \
	"; echo add ip; run add_ip" \
	"; echo add squashfs; run add_squashfs" \
	"; echo booting; booti ${kernel_addr_r} ${ramdisk_addr_r} ${fdt_addr_r}\0" \
	"bootcmd_tftp=echo booting from TFTP, be patient..." \
	"; echo load kernel; run load_kernel_tftp"\
	"; echo load rootfs; run load_rootfs_tftp" \
	"; echo load dtb; run load_dtb_tftp" \
	"; echo add ip; run add_ip" \
	"; echo add squashfs; run add_squashfs" \
	"; echo booting; booti ${kernel_addr_r} ${ramdisk_addr_r} ${fdt_addr_r}\0" \
	"bootcmd_nfs=echo booting from NFS, be patient..." \
	"; echo load kernel; run load_kernel_tftp"\
	"; echo load rootfs; run load_rootfs_tftp" \
	"; echo load dtb; run load_dtb_tftp" \
	"; echo add ip; run add_ip" \
	"; echo add nfs; run add_nfs" \
	"; echo booting; booti ${kernel_addr_r} ${ramdisk_addr_r} ${fdt_addr_r}\0" \
	"bootcmd=run bootcmd_sdc\0" \

#define IMAGE_NAMES \
	"img_dtb_bin=dtb.bin\0" \
	"img_kernel_bin=image.bin\0" \
	"img_rootfs_bin=rootfs.bin\0" \
	"img_squashfs_bin=squashfs.bin\0" \
	"img_uboot_bin=uboot.bin\0"

#define LOAD_CMDS \
	"load_dtb_tftp=tftpboot ${fdt_addr_r} ${tftp_path}${img_dtb_bin}\0" \
	"load_kernel_tftp=tftpboot ${kernel_addr_r} ${tftp_path}${img_kernel_bin}\0" \
	"load_rootfs_tftp=tftpboot ${ramdisk_addr_r} ${tftp_path}${img_rootfs_bin}\0" \
	"load_squashfs_tftp=tftpboot ${ramdisk_addr_r} ${tftp_path}${img_squashfs_bin}\0" \
	"load_uboot_tftp=tftpboot ${kernel_addr_r} ${tftp_path}${img_uboot_bin}\0"

#define UPDATE_CMDS \
	"update_all=run update_uboot; run update_kernel; run update_dtb; run update_rootfs; run update_squashfs\0" \
	"update_dtb=run load_dtb_tftp; fatwrite mmc 0 ${fdt_addr_r} ${img_dtb_bin} ${filesize}\0" \
	"update_kernel=run load_kernel_tftp; fatwrite mmc 0 ${kernel_addr_r} ${img_kernel_bin} ${filesize}\0" \
	"update_rootfs=run load_rootfs_tftp; fatwrite mmc 0 ${ramdisk_addr_r} ${img_rootfs_bin} ${filesize}\0" \
	"update_squashfs=run load_squashfs_tftp; fatwrite mmc 0 ${ramdisk_addr_r} ${img_squashfs_bin} ${filesize}\0" \
	"update_uboot=run load_uboot_tftp; fatwrite mmc 0 ${kernel_addr_r} ${img_uboot_bin} ${filesize}\0"

#define CFG_EXTRA_ENV_SETTINGS \
		BOOT_CMDS \
		IMAGE_NAMES \
		LOAD_CMDS \
		UPDATE_CMDS \
		ENV_MEM_LAYOUT_SETTINGS \
		EXTRA_BOOTENV \
		BOOTENV

#endif /* __ZUKIMO_H */
