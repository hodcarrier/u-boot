/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2020 MediaTek Inc.
 *
 * Author: Gao Weijie <weijie.gao@mediatek.com>
 * based on: include/configs/mt7628.h
 * Copyright (C) 2022-2023 Du Huanpeng <dhu@hodcarrier.org>
 */

#ifndef __CONFIG_LS1C300_H__
#define __CONFIG_LS1C300_H__

#define CFG_SYS_SDRAM_BASE		0x80200000
#define CFG_SYS_INIT_SP_OFFSET		0x2000

/* SPL */
#if defined(CONFIG_SPL_BUILD)

/* Serial SPL */
#if defined(CONFIG_SPL_SERIAL)
#define CFG_SYS_NS16550_CLK		66000000
#define CFG_SYS_NS16550_COM1		0xbfe44000
#define CFG_SYS_NS16550_COM2		0xbfe48000
#define CFG_SYS_NS16550_COM3		0xbfe4c000
#define CFG_SYS_NS16550_COM4		0xbfe4c400
#define CFG_SYS_NS16550_COM5		0xbfe4c500
#define CFG_SYS_NS16550_COM6		0xbfe4c600
#endif

#define CFG_MALLOC_F_ADDR		0x80100000	/* FIXME: find a proper place */
#define CFG_SYS_UBOOT_BASE		0xbd000000

#endif

/* Serial common */
#define CFG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600 }

#endif /* __CONFIG_LS1C300_H__ */
