// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 MediaTek Inc. All Rights Reserved.
 *
 * Author: Gao Weijie <weijie.gao@mediatek.com>
 *
 * base on: drivers/clk/mtmips/clk-mt7628.c
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


#define CLKCFG1_REG 0

struct ls1c300_clk_priv {
	void __iomem *base;
	int xtal_clk;
	int pll_clk;
	int cpu_clk;
	union {
		int sdram_clk;
		int apb_clk;
	};
};


const struct clk_div_table sdram_div[] = {
	{.val = 0, .div = 2},
	{.val = 1, .div = 4},
	{.val = 2, .div = 3},
	{.val = 3, .div = 3},
};


static ulong ls1c300_clk_get_rate(struct clk *clk)
{
	struct ls1c300_clk_priv *priv = dev_get_priv(clk->dev);
	ulong ret;



	switch (clk->id) {
	case CLK_XTAL:
		ret = priv->xtal_clk;
		break;
	case CLK_CPU:
		ret = priv->cpu_clk;
		break;
	case CLK_SDRAM:
		ret = priv->sdram_clk;
		break;
	default:
		ret = 0;
	}

	return ret;
}

static int ls1c300_clk_enable(struct clk *clk)
{
	struct ls1c300_clk_priv *priv = dev_get_priv(clk->dev);

	if (clk->id > 31)
		return -1;

	setbits_32(priv->base + CLKCFG1_REG, BIT(clk->id));

	return 0;
}

static int ls1c300_clk_disable(struct clk *clk)
{
	struct ls1c300_clk_priv *priv = dev_get_priv(clk->dev);

	if (clk->id > 31)
		return -1;

	clrbits_32(priv->base + CLKCFG1_REG, BIT(clk->id));

	return 0;
}

const struct clk_ops ls1c300_clk_ops = {
	.enable = ls1c300_clk_enable,
	.disable = ls1c300_clk_disable,
	.get_rate = ls1c300_clk_get_rate,
};


// Freq_PLL = XIN *(M_PLL + FRAC_N)/4
static int ls1c300_clk_probe(struct udevice *dev)
{
	struct ls1c300_clk_priv *priv = dev_get_priv(dev);
	void __iomem *cpu_throt;

	unsigned int pll, div;

	priv->base = (void *)dev_remap_addr_index(dev, 0);
#define START_FREQ	0
#define CLK_DIV_PARAM	4
	cpu_throt  = (void *)dev_remap_addr_index(dev, 1);

	pll = readl(priv->base + START_FREQ);
	div = readl(priv->base + CLK_DIV_PARAM);

	debug("   START_FREQ: [%08x]\n", pll);
	debug("CLK_DIV_PARAM: [%08x]\n", div);

	priv->pll_clk = (pll >> 8) & 255;

	priv->xtal_clk  =  24000000;
	priv->pll_clk  *= priv->xtal_clk / 4;

	priv->cpu_clk   = priv->pll_clk / 2;
	priv->sdram_clk = priv->cpu_clk / 2;
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
	.priv_auto = sizeof(struct ls1c300_clk_priv),
	.ops = &ls1c300_clk_ops,
};
