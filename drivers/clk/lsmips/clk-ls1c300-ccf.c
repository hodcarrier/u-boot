// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2022 Du Huanpeng <dhu@hodcarrier.org>
 */

#include <linux/types.h>
#include <dm/device.h>

#include <linux/clk-provider.h>
#include <dt-bindings/clock/ls1c300-clk.h>

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

// private data
struct ls1c300_ccf_clk_priv {
	int notused;
};

// xtal fixed-clock
struct clk_fixed_rate clk_xtal = {
	.fixed_rate = 24000000
};

// pll clk-privider
struct clk_fixed_factor clk_pll = {
	.mult = 22,
	.div  = 4
};

// cpu clk_mux
static const char * const cpu_parent_names[] = {"xtal", "pll"};
struct clk_mux clk_cpu = {
	.reg = (void *)0xbfe78030,
	.table = NULL,
	.mask  = CPU_SEL,
	.shift = 1,
	.flags = 0,
	.parent_names = cpu_parent_names,
	.num_parents = 2
};

// sdram clk_divider
const struct clk_div_table div_table_sdram[] = {
	{.val = 0, .div = 2},
	{.val = 1, .div = 4},
	{.val = 2, .div = 3},
	{.val = 3, .div = 3},
};

struct clk_divider clk_sdram = {
	.reg = (void *)0xbfe78034,
	.shift = 1,
	.width = 1,
	.flags = 1,
	.table = div_table_sdram,
};
// TODO: mac clk_xxx
struct clk_fixed_factor clk_mac = {
	.mult = 1,
	.div  = 1,
};

int ls1c300_ccf_of_xlate(struct clk *clock, struct ofnode_phandle_args *args)
{
	printf("DBG: %s: %p\n", __func__, clock);
	return 0;
}

ulong ls1c300_ccf_get_rate(struct clk *clk)
{
	printf("DBG: %s: %p\n", __func__, clk);

	if(clk->id == CLK_XTAL)   return  24000000;
	if(clk->id == CLK_CPU)    return 252000000;
	if(clk->id == CLK_SDRAM)  return 126000000;
	return 0;
}

int ls1c300_ccf_set_parent(struct clk *clk, struct clk *parent)
{
	printf("DBG: %s: %p\n", __func__, clk);
	return 0;
}

struct clk_ops ls1c300_ccf_clk_ops = {
	.of_xlate = ls1c300_ccf_of_xlate,
	.get_rate = ls1c300_ccf_get_rate,
};

int ls1c300_clk_ccf_probe(struct udevice *dev)
{
	printf("DBG: %s: %p\n", __func__, dev);
	return 0;
}

static const struct udevice_id ls1c300_ccf_clk_ids[] = {
	{ .compatible = "loongson,ls1c300-clk-ccf" },
	{ }
};

U_BOOT_DRIVER(ls1c300_clk_ccf) = {
	.name = "ls1c300-clk-ccf",
	.id = UCLASS_CLK,
	.of_match = ls1c300_ccf_clk_ids,
	.probe = ls1c300_clk_ccf_probe,
	.priv_auto = sizeof(struct ls1c300_ccf_clk_priv),
	.ops = &ls1c300_ccf_clk_ops,
};
