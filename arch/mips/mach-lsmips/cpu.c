// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2018 Stefan Roese <sr@denx.de>
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
#ifdef CONFIG_SKIP_LOWLEVEL_INIT
	gd->ram_size = get_ram_size((void *)CONFIG_SYS_SDRAM_BASE, SZ_256M);
#else
	gd->ram_size = SZ_64M;
#endif

	return 0;
}
