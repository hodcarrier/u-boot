// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2020 MediaTek Inc.
 *
 * Author:  Gao Weijie <weijie.gao@mediatek.com>
 *
 * based on: arch/mips/mach-mtmips/mt7628/init.c
 * Copyright (C) 2020-2022 Du Huanpeng <dhu@hodcarrier.org>
 */

#include <common.h>
#include <clk.h>
#include <dm.h>
#include <dm/uclass.h>
#include <dt-bindings/clock/ls1c300-clk.h>
#include <linux/io.h>
#include <linux/sizes.h>

DECLARE_GLOBAL_DATA_PTR;

int print_cpuinfo(void)
{
	struct udevice *udev;
	struct clk clk;
	int ret;
	ulong xtal;
	char buf[SZ_32];

	printf("CPU: Loongson ls1c300b\n");

	ret = uclass_get_device_by_driver(UCLASS_CLK, DM_DRIVER_GET(ls1c300_clk), &udev);

	if (ret) {
		printf("error: clock driver not found.\n");
		return 0;
	}

	clk.dev = udev;

	ret = clk_request(udev, &clk);
	if(ret < 0)
		return ret;

	clk.id = CLK_XTAL;
	xtal = clk_get_rate(&clk);

	clk.id = CLK_CPU_THROT;
	gd->cpu_clk = clk_get_rate(&clk);

	clk.id = CLK_APB;
	gd->mem_clk = clk_get_rate(&clk);

	printf("Clock: CPU: %sMHz, ", strmhz(buf, gd->cpu_clk));
	printf("SDRAM: %sMHz, ", strmhz(buf, gd->mem_clk));
	printf("XTAL: %sMHz\n", strmhz(buf, xtal));

	return 0;
}

ulong notrace get_tbclk(void)
{
	return gd->cpu_clk / 2;
}
