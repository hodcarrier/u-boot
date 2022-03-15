// SPDX-License-Identifier: GPL-2.0
/*
 *
 * reference code:
 * 1. drivers/clk/mtmips/clk-mt7628.c
 * 2. drivers/clk/clk_octeon.c
 * 3. drivers/clk/microchip/mpfs_clk.c
 *
 * Copyright (C) 2020-2022 Du Huanpeng <dhu@hodcarrier.org>
 */

#include <common.h>
#include <clk-uclass.h>
#include <dm.h>
#include <dt-bindings/clock/ls1c300-clk.h>
#include <linux/bitops.h>
#include <linux/io.h>
#include <linux/clk-provider.h>

/* START_FREQ */
#define	PLL_VALID	BIT(31)
#define	RESERVED0	GENMASK(30,24)
#define	FRAC_N		GENMASK(23,16)
#define	M_PLL		GENMASK(15,8)
#define	RESERVED1	GENMASK(7,4)
#define	RST_TIME	GENMASK(3,2)
#define	SDRAM_DIV	GENMASK(1,0)
/* CLK_DIV_PARAM */
#define	PIX_DIV		GENMASK(31,24)
#define	CAM_DIV		GENMASK(23,16)
#define	CPU_DIV		GENMASK(15,8)
#define	RESERVED2	GENMASK(7,6)
#define	PIX_DIV_VALID	BIT(5)
#define	PIX_SEL		BIT(4)
#define	CAM_DIV_VALID	BIT(3)
#define	CAM_SEL		BIT(2)
#define	CPU_DIV_VALID	BIT(1)
#define	CPU_SEL		BIT(0)
/* CPU_THROT */
#define	CPU_THROT	GENMASK(3,0)


const struct clk_div_table sdram_div[] = {
	{.val = 0, .div = 2},
	{.val = 1, .div = 4},
	{.val = 2, .div = 3},
	{.val = 3, .div = 3},
};

static ulong ls1c300_clk_get_rate(struct clk *clk)
{
	struct clk *c;
	ulong rate;
	int err;

	err = clk_get_by_id(clk->id, &c);
	if (err)
		return err;

	rate = clk_get_rate(c);
	return rate;
}

int ls1c300_set_parent(struct clk *clk, struct clk *parent)
{
	printf("CLK: %s\n", __func__);
	return 0;
}


static int ls1c300_clk_enable(struct clk *clk)
{
	printf("%s id:[%ld]\n", __func__, clk->id);
	return 0;
}

static int ls1c300_clk_disable(struct clk *clk)
{
	printf("%s id:[%ld]\n", __func__, clk->id);
	return 0;
}

const struct clk_ops ls1c300_clk_ops = {
	.enable = ls1c300_clk_enable,
	.disable = ls1c300_clk_disable,
	.get_rate = ls1c300_clk_get_rate,
};


static int ls1c300_clk_probe(struct udevice *dev)
{
	void __iomem *base;
	void __iomem *cpu_throt;

	struct clk *cl, clk;

	int ret;
	const char *parent_name;
	unsigned int mult;
	unsigned int pll, div;

	base = (void *)dev_remap_addr_index(dev, 0);
#define START_FREQ	0
#define CLK_DIV_PARAM	4
	cpu_throt  = (void *)dev_remap_addr_index(dev, 1);

	debug("     base: %p\n", base);
	debug("cpu_throt: %p\n", cpu_throt);
	pll = readl(base + START_FREQ);
	div = readl(base + CLK_DIV_PARAM);
	debug("   START_FREQ: [%08x]\n", pll);
	debug("CLK_DIV_PARAM: [%08x]\n", div);

	ret = clk_get_by_index(dev, 0, &clk);
	if(ret)
		return ret;

	ret = clk_get_rate(&clk);

	parent_name = clk.dev->name;

	mult = (pll >> 8) % 256 + (pll >> 16) % 256;
//	readl(cpu_throt) & 15;

	cl = clk_register_fixed_factor(NULL, "pll", parent_name, 0, mult, 4);
	clk_dm(CLK_PLL, cl);
	cl = clk_register_fixed_factor(NULL, "cpu", "pll", 0, 1, 2);
	clk_dm(CLK_CPU, cl);
	cl = clk_register_fixed_factor(NULL, "sdram", "cpu", 0, 1, 2);
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
