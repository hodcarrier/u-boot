// SPDX-License-Identifier: GPL-2.0
/*
 * reference:
 *   drivers/clk/microchip/mpfs_clk.c
 *   drivers/clk/clk_octeon.c
 *
 * Copyright (C) 2020-2022 Du Huanpeng <dhu@hodcarrier.org>
 */

#include <common.h>
#include <clk-uclass.h>
#include <dm.h>
#include <dt-bindings/clock/ls1c300-clk.h>
#include <linux/bitops.h>
#include <linux/bitfield.h>
#include <linux/io.h>
#include <linux/clk-provider.h>

/* START_FREQ */
#define	PLL_VALID	BIT(31)
#define	RESERVED0	GENMASK(30, 24)
#define	FRAC_N		GENMASK(23, 16)
#define	M_PLL		GENMASK(15, 8)
#define	RESERVED1	GENMASK(7, 4)
#define	RST_TIME	GENMASK(3, 2)
#define	SDRAM_DIV	GENMASK(1, 0)
/* CLK_DIV_PARAM */
#define	PIX_DIV		GENMASK(31, 24)
#define	CAM_DIV		GENMASK(23, 16)
#define	CPU_DIV		GENMASK(15, 8)
#define	RESERVED2	GENMASK(7, 6)
#define	PIX_DIV_VALID	BIT(5)
#define	PIX_SEL		BIT(4)
#define	CAM_DIV_VALID	BIT(3)
#define	CAM_SEL		BIT(2)
#define	CPU_DIV_VALID	BIT(1)
#define	CPU_SEL		BIT(0)
/* CPU_THROT */
#define	CPU_THROT	GENMASK(3, 0)

static const struct clk_div_table sdram_div_table[] = {
	{.val = 0, .div = 2},
	{.val = 1, .div = 4},
	{.val = 2, .div = 3},
	{.val = 3, .div = 3},
};

static ulong ls1c300_clk_get_rate(struct clk *clk)
{
	struct clk *cl;
	ulong rate;
	int err;

	err = clk_get_by_id(clk->id, &cl);
	if (err)
		return err;

	rate = clk_get_rate(cl);
	return rate;
}

static int ls1c300_clk_enable(struct clk *clk)
{
	/* Nothing to do on Octeon */
	return 0;
}

static const struct clk_ops ls1c300_clk_ops = {
	.enable = ls1c300_clk_enable,
	.get_rate = ls1c300_clk_get_rate,
};

static int ls1c300_clk_probe(struct udevice *dev)
{
	void __iomem *base;
	void __iomem *cpu_throt;
	void __iomem *addr;

	struct clk *cl, clk;

	int ret;
	const char *parent_name;
	unsigned int mult, div;
	unsigned int val;
	int flags;

	base = (void *)dev_remap_addr_index(dev, 0);
#define START_FREQ	(0)
#define CLK_DIV_PARAM	(4)
	cpu_throt  = (void *)dev_remap_addr_index(dev, 1);

	debug("     base: %p\n", base);
	debug("cpu_throt: %p\n", cpu_throt);

	val = readl(base + START_FREQ);
	debug("   START_FREQ: [%08x]\n", val);
	val = readl(base + CLK_DIV_PARAM);
	debug("CLK_DIV_PARAM: [%08x]\n", val);

	ret = clk_get_by_index(dev, 0, &clk);
	if (ret)
		return ret;

	ret = clk_get_rate(&clk);

	parent_name = clk.dev->name;
	val = readl(base + START_FREQ);
	mult = FIELD_GET(FRAC_N, val) + FIELD_GET(M_PLL, val); div = 4;
	cl = clk_register_fixed_factor(NULL, "pll", parent_name, 0, mult, div);
	clk_dm(CLK_PLL, cl);

	addr = base + CLK_DIV_PARAM;
	flags = CLK_DIVIDER_ONE_BASED | CLK_DIVIDER_ALLOW_ZERO;
	cl = clk_register_divider(NULL, "cpu_div", "pll", 0, addr,  8, 7, flags);
	clk_dm(CLK_CPU, cl);
	cl = clk_register_divider(NULL, "cam_div", "pll", 0, addr, 16, 7, flags);
	clk_dm(CLK_CAMERA, cl);
	cl = clk_register_divider(NULL, "pix_div", "pll", 0, addr, 24, 7, flags);
	clk_dm(CLK_PIX, cl);

	mult = FIELD_GET(CPU_THROT, readl(cpu_throt)) + 1; div = 16;
	cl = clk_register_fixed_factor(NULL, "cpu_throt_factor", "cpu_div", CLK_GET_RATE_NOCACHE, mult, div);
	clk_dm(CLK_CPU_THROT, cl);

	addr = base + START_FREQ;
	cl = clk_register_divider(NULL, "sdram_div", "cpu_div", 0, addr, 0, 2, 0);
	to_clk_divider(cl)->table = sdram_div_table;
	clk_dm(CLK_SDRAM, cl);

	return 0;
}

static const struct udevice_id ls1c300_clk_ids[] = {
	{ .compatible = "loongson,ls1c300-clk" },
	{ }
};

U_BOOT_DRIVER(ls1c300_clk) = {
	.name = "ls1c300-clk",
	.id = UCLASS_CLK,
	.of_match = ls1c300_clk_ids,
	.probe = ls1c300_clk_probe,
	.priv_auto = sizeof(struct clk),
	.ops = &ls1c300_clk_ops,
};
