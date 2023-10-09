// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2018 Stefan Roese <sr@denx.de>
 * Copyright (C) 2022-2023 Du Huanpeng <dhu@hodcarrier.org>
 */

#include <common.h>
#include <init.h>
#include <malloc.h>
#include <linux/bitops.h>
#include <linux/io.h>
#include <linux/sizes.h>

DECLARE_GLOBAL_DATA_PTR;

int dram_init(void)
{
	// FIXME: 配合 CFG_SYS_SDRAM_BASE 设置正确的 ram_size
	gd->ram_size = get_ram_size((void *)KSEG1, SZ_256M) - CONFIG_TEXT_BASE % SZ_256M;
	return 0;
}
