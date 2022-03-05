// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2020 MediaTek Inc.
 *
 * Author:  Gao Weijie <weijie.gao@mediatek.com>
 *
 * based on: arch/mips/mach-mtmips/mt7628/init.c
 * Du Huanpeng <dhu@hodcarrier.org>
 */

#include <common.h>
#include <clk.h>
#include <dm.h>
#include <dm/uclass.h>
#include <dt-bindings/clock/ls1c300-clk.h>
#include <linux/io.h>
#include <linux/sizes.h>
#include "ls1c300.h"

DECLARE_GLOBAL_DATA_PTR;


int print_cpuinfo(void)
{
	struct udevice *clkdev;
	ulong cpu_clk, sdram_clk, xtal_clk;
	struct clk clk;
	int ret;

	printf("CPU: Loongson ls1c300b ver: xxx\n");
	printf("Boot: SDRAM , SPI-NOR xx-Byte Addr, CPU clock from PLL\n");

	ret = uclass_get_device_by_driver(UCLASS_CLK, DM_DRIVER_GET(ls1c300_clk), &clkdev);

	if (ret) {
		return ret;
	}

	clk.dev = clkdev;

	clk.id = CLK_XTAL;
	xtal_clk = clk_get_rate(&clk);

	clk.id = CLK_CPU;
	cpu_clk = clk_get_rate(&clk);

	clk.id = CLK_SDRAM;
	sdram_clk = clk_get_rate(&clk);

	printf("Clock: CPU: %luMHz, SDRAM: %luMHz, XTAL: %luMHz\n",
	       cpu_clk / 1000000, sdram_clk / 1000000, xtal_clk / 1000000);

	return 0;
}


ulong notrace get_tbclk(void)
{
	/* TODO: recalculate cpu freq. */
	return CONFIG_SYS_MIPS_TIMER_FREQ;
}
