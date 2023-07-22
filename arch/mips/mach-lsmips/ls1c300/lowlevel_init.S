/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2020 MediaTek Inc.
 *
 * Author:  Gao Weijie <weijie.gao@mediatek.com>
 *
 * Copyright (C) 2020-2023 Du Huanpeng <dhu@hodcarrier.org>
 */

#include <config.h>
#include <asm-offsets.h>
#include <asm/cacheops.h>
#include <asm/regdef.h>
#include <asm/mipsregs.h>
#include <asm/addrspace.h>
#include <asm/asm.h>
#include <linux/sizes.h>

/* PLL control register */
#define NAND_BASE	0xbfe70000
#define START_FREQ	0x8030
#define CLK_DIV_PARAM	0x8034
#define CPU_THROT	0xc010

/* START_FREQ */
#define PLL_VALID	31
#define Reserved_24	24
#define FRAC_N		16
#define M_PLL		8
#define Reserved_4	4
#define RST_TIME	2
#define SDRAM_DIV	0
 #define SDRAM_DIV2	0
 #define SDRAM_DIV4	1
 #define SDRAM_DIV3	2

/* CLK_DIV_PARAM */
#define PIX_DIV		24
#define CAM_DIV		16
#define CPU_DIV		8
#define PIX_DIV_VALID	5
#define PIX_SEL		4
#define CAM_DIV_VALID	3
#define CAM_SEL		2
#define CPU_DIV_VALID	1
#define CPU_SEL		0

/* Document:
 * Freq_PLL = XIN *(M_PLL + FRAC_N)/4
 */
#define XIN			24000000
#define PLL_VALID_1		(1<<PLL_VALID)
#define PREP_M_PLL(Freq_PLL)	(((Freq_PLL * 4) / XIN) << M_PLL)
#define PREP_SDRAM_DIV(div)	(div<<SDRAM_DIV)
#define PREP_CPU_DIV(div1)	((0x80|div1)<<CPU_DIV | (div1&&div1)<<CPU_DIV_VALID)
#define PREP_PIX_DIV(div2)	(div2<<PIX_DIV)
#define PREP_CAM_DIV(div3)	(div3<<CAM_DIV)

/* PLL @264MHz, CPU @132MHz, SDRAM @66MHz */
#define CFG_START_FREQ		(PLL_VALID_1 | PREP_M_PLL(264000000) | SDRAM_DIV2)
#define CFG_CLK_DIV_PARAM	(PREP_CPU_DIV(2) | PREP_PIX_DIV(0x24) | PREP_CAM_DIV(0x24))
#define CFG_CPU_THROT		15

/* SPI0 control register */
#define SPI0_BASE		0xbfe80000
#define SPCR			0
#define SPSR			1
#define TxFIFO			2
#define RxFIFO			2
#define SPER			3
#define SFC_PARAM		4
    #define CLK_DIV		4
    #define DUAL_IO		3
    #define FAST_READ		2
    #define BURST_EN		1
    #define MEMORY_EN		0
#define SFC_SOFTCS		5
#define SFC_TIMING		6
    #define T_FAST		2
    #define T_CSH		0

	.set noreorder
LEAF(ls1c300_pll_init)
#if !CONFIG_IS_ENABLED(SKIP_LOWLEVEL_INIT)
	li	t0, NAND_BASE
	li	t1, CFG_START_FREQ
	li	t2, CFG_CLK_DIV_PARAM
	li	t3, CFG_CPU_THROT

	sw	t3, CPU_THROT (t0)
	sw	t2, CLK_DIV_PARAM (t0)
	sw	t1, START_FREQ (t0)

	ori	t2, 1<<CPU_SEL
	sw	t2, CLK_DIV_PARAM (t0)
#endif
	li	v0, 264000000
	jr	ra
	 nop
END(ls1c300_pll_init)

LEAF(ls1c300_spi_init)
#if !CONFIG_IS_ENABLED(SKIP_LOWLEVEL_INIT)
	li	t0, SPI0_BASE
	li	t1, (1<<MEMORY_EN) | (1<<BURST_EN) | (1<<FAST_READ) | (1<<DUAL_IO)
	sb	t1, SFC_PARAM (t0)
	li	t2, (1<<T_FAST) | (1<<T_CSH)
	sb	t2, SFC_TIMING (t0)
#endif
	jr	ra
	  nop
END(ls1c300_spi_init)

NESTED(lowlevel_init, 0, ra)
	/* Save ra and do real lowlevel initialization */
	move	s0, ra
	/* Setup PLL @264MHz */
	PTR_LA	t9, ls1c300_pll_init
	jalr	t9
	  nop

	/* Setup SPI Dual IO@33MHz */
	PTR_LA	t9, ls1c300_spi_init
	jalr	t9
	  nop

	/* Setup external SDRAM @66MHz */
	PTR_LA	t9, ls1c300_sdram_init
	jalr	t9
	  nop

	move	ra, s0
	jr	ra
	 nop
END(lowlevel_init)
