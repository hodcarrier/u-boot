// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2020 MediaTek Inc. All Rights Reserved.
 *
 * Author: Gao Weijie <weijie.gao@mediatek.com>
 *
 * Copyright (C) 2022 Du Huanpeng <dhu@hodcarrier.org>
 */

#include <common.h>
#include <init.h>
#include <spl.h>
#include <asm/sections.h>
#include <linux/libfdt.h>
#include <linux/sizes.h>
#include <mach/serial.h>

void __noreturn board_init_f(ulong dummy)
{
	spl_init();

#ifdef CONFIG_SPL_SERIAL
	/*
	 * lsmips_spl_serial_init() is useful if debug uart is enabled,
	 * or DM based serial is not enabled.
	 */
	lsmips_spl_serial_init();
	preloader_console_init();
#endif

	board_init_r(NULL, 0);
}

void board_boot_order(u32 *spl_boot_list)
{
	spl_boot_list[0] = BOOT_DEVICE_NOR;
}

unsigned long spl_nor_get_uboot_base(void)
{
	void *uboot_base = __image_copy_end;

	if (fdt_magic(uboot_base) == FDT_MAGIC)
		return (unsigned long)uboot_base + fdt_totalsize(uboot_base);

	return (unsigned long)uboot_base;
}
