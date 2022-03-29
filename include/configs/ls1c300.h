/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2020 MediaTek Inc.
 *
 * Author: Gao Weijie <weijie.gao@mediatek.com>
 * based on: include/configs/mt7628.h
 * Copyright (C) 2022 Du Huanpeng <dhu@hodcarrier.org>
 */

#ifndef __CONFIG_LS1C300_H__
#define __CONFIG_LS1C300_H__

#define CONFIG_SYS_HZ			1000

#define CONFIG_SYS_MONITOR_BASE		CONFIG_SYS_TEXT_BASE

#define CONFIG_SYS_BOOTPARAMS_LEN	0x20000

#define CONFIG_SYS_SDRAM_BASE		0x80000000
#define CONFIG_SYS_LOAD_ADDR		0x80010000

#define CONFIG_SYS_INIT_SP_OFFSET	0x80000

#define CONFIG_SYS_BOOTM_LEN		0x1000000

#define CONFIG_SYS_MAXARGS		16
#define CONFIG_SYS_CBSIZE		1024

/* Serial SPL */
#define CONFIG_SYS_NS16550_SERIAL
#if defined(CONFIG_SPL_BUILD) && defined(CONFIG_SPL_SERIAL)
#define CONFIG_SYS_NS16550_CLK		66000000
#define CONFIG_SYS_NS16550_REG_SIZE	(-1)
#define CONFIG_SYS_NS16550_COM1		0xbfe44000
#define CONFIG_SYS_NS16550_COM2		0xbfe48000
#define CONFIG_SYS_NS16550_COM3		0xbfe4c000
#define CONFIG_SYS_NS16550_COM4		0xbfe4c400
#define CONFIG_SYS_NS16550_COM5		0xbfe4c500
#define CONFIG_SYS_NS16550_COM6		0xbfe4c600
#endif

/* Serial common */
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600 }

/* SPL */
#if defined(CONFIG_SPL) && !defined(CONFIG_SPL_BUILD)
#define CONFIG_SKIP_LOWLEVEL_INIT
#endif

#define CONFIG_SYS_UBOOT_START		CONFIG_SYS_TEXT_BASE
#define CONFIG_SPL_BSS_START_ADDR	0x80010000
#define CONFIG_SPL_BSS_MAX_SIZE		0x10000
#define CONFIG_SPL_MAX_SIZE		0x10000
#define CONFIG_SPL_PAD_TO		0

#define CONFIG_MALLOC_F_ADDR		0x80100000	/* FIXME: find a proper place */

/* Dummy value */
#define CONFIG_SYS_UBOOT_BASE		0

#endif /* __CONFIG_LS1C300_H__ */
