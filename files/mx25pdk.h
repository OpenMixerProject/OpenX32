/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2011 Freescale Semiconductor, Inc.
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include <asm/arch/imx-regs.h>

/* High Level Configuration Options */

#define CONFIG_SYS_TEXT_BASE		0x800000C0
#define CONFIG_SYS_FSL_CLK

#define CONFIG_SYS_TIMER_RATE		32768
#define CONFIG_SYS_TIMER_COUNTER	\
	(&((struct gpt_regs *)IMX_GPT1_BASE)->counter)

#define CONFIG_CMDLINE_TAG		/* enable passing of ATAGs */
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG

#define CONFIG_MACH_TYPE	MACH_TYPE_MX25_3DS

/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + 2 * 1024 * 1024)

/* Physical Memory Map */

#define PHYS_SDRAM_1		0x80000000
#define PHYS_SDRAM_1_SIZE	(64 * 1024 * 1024)

#define CONFIG_SYS_SDRAM_BASE		PHYS_SDRAM_1
#define CONFIG_SYS_INIT_RAM_ADDR	IMX_RAM_BASE
#define CONFIG_SYS_INIT_RAM_SIZE	IMX_RAM_SIZE

#define CONFIG_SYS_INIT_SP_OFFSET \
	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_SP_OFFSET)

/* Memory Test */

/* Serial Info */
#define CONFIG_MXC_UART_BASE	UART5_BASE
#define CONFIG_OF_STDOUT_VIA_ALIAS

/* No NOR flash present */

/* U-Boot general configuration */
#define CONFIG_SYS_CBSIZE	1024	/* Console I/O Buffer Size  */
/* Boot Argument Buffer Size */
#define CONFIG_SYS_BARGSIZE	CONFIG_SYS_CBSIZE

/* Ethernet */
#define CONFIG_FEC_MXC
#define CONFIG_FEC_MXC_PHYADDR		0x1f

/* ESDHC driver */
#define CONFIG_SYS_FSL_ESDHC_ADDR	IMX_MMC_SDHC1_BASE
#define CONFIG_SYS_FSL_ESDHC_NUM	1

/* PMIC Configs */
#define CONFIG_POWER
#define CONFIG_POWER_I2C
#define CONFIG_POWER_FSL
#define CONFIG_POWER_FSL_MC34704
#define CONFIG_SYS_FSL_PMIC_I2C_ADDR	0x54

/* I2C Configs */
#define CONFIG_SYS_I2C
#define CONFIG_SYS_I2C_MXC
#define CONFIG_SYS_I2C_MXC_I2C1		/* enable I2C bus 1 */
#define CONFIG_SYS_I2C_MXC_I2C2		/* enable I2C bus 2 */

/* RTC */
#define CONFIG_RTC_IMXDI

/* Fuse API support */
#define CONFIG_FSL_IIM

/* Ethernet Configs */

/* Framebuffer support */
#ifdef CONFIG_VIDEO
#define CONFIG_VIDEO_LOGO
#define CONFIG_BMP_16BPP
#define CONFIG_VIDEO_BMP_RLE8
#define CONFIG_VIDEO_BMP_GZIP
#define CONFIG_SYS_VIDEO_LOGO_MAX_SIZE  (512 << 10)
#endif

/* Boot Linux */
#define CONFIG_BOOTFILE         "uImage"
#define CONFIG_LOADADDR		0x80060000	/* loadaddr env var */
#define CONFIG_SYS_LOAD_ADDR	CONFIG_LOADADDR

//#define CONFIG_BOOTARGS "console=ttymxc4,115200 root=/dev/ram0 rw init=/init initrd=0x80810000 video=imxfb:800x480@60"
#define CONFIG_BOOTARGS "console=ttymxc4,115200 console=tty0 root=/dev/ram0 rw init=/init initrd=0x80810000 video=imxfb:800x480 vt.global_cursor_default=0"
#define CONFIG_EXTRA_ENV_SETTINGS "bootargs=" CONFIG_BOOTARGS "\0"
/* bootm <kernel_addr> <ramdisk_addr> <dtb_addr> */
#define CONFIG_BOOTCOMMAND "echo Loading kernel from RAM...; bootm 0x80060000 0x80810000 0x80800000"

/* Miscellaneous configurable options */

#endif /* __CONFIG_H */
