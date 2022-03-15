// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2022 Du Huanpeng <dhu@hodcarrier.org>
 */

#include <linux/types.h>
#include <dm/device.h>

#include <linux/clk-provider.h>
#include <dt-bindings/clock/ls1c300-clk.h>
#include <linux/bitops.h>

#include <asm/io.h>


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
	.fixed_rate = 24000016
};

// pll clk-privider
struct clk_fixed_factor clk_pll = {
	.mult = 0x2C,
	.div  = 4,
};

// Freq_PLL = XIN *(M_PLL + FRAC_N)/4


// cpu clk_mux
static const char * const cpu_parent_names[] = {"xtal", "pll"};
struct clk_mux clk_cpu = {
	.reg = (void *)0xbfe78034,
	.table = NULL,
	.mask  = CPU_SEL,
	.shift = 8,
	.flags = 0,
	.parent_names = cpu_parent_names,
	.num_parents = 2,
};

struct clk_divider clk_cpu_div = {
	.reg = (void *)0xbfe78034,
	.shift = 8,
	.width = 7,
	.flags = CLK_DIVIDER_ONE_BASED,
	.table = NULL,
};

// sdram clk_divider
const struct clk_div_table div_table_sdram[] = {
	{.val = 0, .div = 2},
	{.val = 1, .div = 4},
	{.val = 2, .div = 3},
	{.val = 3, .div = 3},
};

struct clk_divider clk_sdram = {
	.reg = (void *)0xbfe78030,
	.shift = 0,
	.width = 2,
	.flags = 0,
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

#if 1
ulong ls1c300_ccf_get_rate(struct clk *clk)
{
	printf("DBG: %s: %p\n", __func__, clk);
	printf("DBG: %s\n", clk->dev->name);

	int err;
	struct clk *cl;


if(clk->id == CLK_XTAL)   { printf("CLK_XTAL  :[%d]\n", __LINE__); return  24000000;}
if(clk->id == CLK_CPU)    { printf("CLK_CPU   :[%d]\n", __LINE__); return 264000000;}
if(clk->id == CLK_SDRAM)  { printf("CLK_SDRAM :[%d]\n", __LINE__); return 132000000;}
return 0;


	if(clk->id == CLK_XTAL) {
		err = clk_get_rate(clk);
		return err;
	}

	if(clk->id == CLK_PLL) {
		cl = dev_get_clk_ptr(clk->dev->parent);
		err = clk_get_rate(cl);

		return 252000000;
	}

	if(clk->id == CLK_SDRAM) return 126000000;

	return 0;

//	if(clk->id == CLK_XTAL)   return  24010888;
//	if(clk->id == CLK_CPU)    return 252000000;
//	if(clk->id == CLK_SDRAM)  return 126000000;
//	return 0;
}
#endif

int ls1c300_ccf_set_parent(struct clk *clk, struct clk *parent)
{
	struct clk *cl;

	printf("DBG: %s: %p\n", __func__, clk);
	return 0;
}



struct clk_ops ls1c300_ccf_clk_ops = {
	.of_xlate = ls1c300_ccf_of_xlate,
	.get_rate = ls1c300_ccf_get_rate,

};

int ls1c300_clk_ccf_probe(struct udevice *dev)
{
	struct clk *cl;

	int err;


	cl = dev_get_priv(dev);


//	cl = &clk_xtal.clk;
//	err = clk_get_by_index(dev, 0, cl);
//	err = clk_get_by_id(cl->id, &cl);
//	clk_xtal.fixed_rate = clk_get_rate(cl);

//	cl = &clk_xtal.clk;
//	err = clk_get_by_index(dev, CLK_XTAL, cl);
//
//	cl = &clk_pll.clk;
//	err = clk_get_by_index(dev, CLK_PLL, cl);
//	err = clk_set_parent(cl, &clk_xtal.clk);
//	clk_pll.mult = 1;
//	clk_pll.div  = 1;
//
//	cl = &clk_cpu.clk;
//	err = clk_get_by_index(dev, CLK_CPU, cl);
//	err = clk_set_parent(cl, &clk_pll.clk);


//	cl = clk_register_fixed_rate(NULL, "xtal", clk_xtal.fixed_rate);

	err = readl((void *)0xbfe78030);

	// clk_pll.mult = (err >> 8) + (err >> 16);
	clk_pll.mult  = (err >> 8)  & 255;
	clk_pll.mult += (err >> 16) & 255;
	clk_pll.div  = 4;

	printf("PLL: %d MHz\n", 24 * clk_pll.mult / clk_pll.div);
	cl = clk_register_fixed_rate(NULL, "xtal", 24000000);
	cl = clk_register_fixed_factor(NULL, "pll", "oscillator@0", CLK_RECALC_NEW_RATES, clk_pll.mult, clk_pll.div);

	cl = clk_register_divider(NULL, "cpu", "pll", CLK_RECALC_NEW_RATES, clk_cpu_div.reg, clk_cpu_div.shift, clk_cpu_div.width, clk_cpu_div.flags);
	cl = clk_register_divider(NULL, "sdram", "cpu", CLK_RECALC_NEW_RATES, clk_sdram.reg, clk_sdram.shift, clk_sdram.width, clk_sdram.flags);



	printf("xtal: %lldMHz\n", cl->rate);
	// printf("xtal: %ldMHz\n", clk_xtal.fixed_rate);


	printf("DBG: %s: %p\n", __func__, dev);
	err = 0;
	return err;
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
