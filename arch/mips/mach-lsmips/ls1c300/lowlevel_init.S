/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2020 MediaTek Inc.
 *
 * Author:  Gao Weijie <weijie.gao@mediatek.com>
 *
 * Copyright (C) 2020-2022 Du Huanpeng <dhu@hodcarrier.org>
 */

#include <config.h>
#include <asm-offsets.h>
#include <asm/cacheops.h>
#include <asm/regdef.h>
#include <asm/mipsregs.h>
#include <asm/addrspace.h>
#include <asm/asm.h>
#include <linux/sizes.h>

/* PLL */
#define NAND_BASE	0xbfe70000
#define START_FREQ	0x8030
#define CLK_DIV_PARAM	0x8034
#define CPU_THROT	0xc010
#define CPU_SEL		0x1

#define CFG_START_FREQ		2147494924
#define CFG_CLK_DIV_PARAM	555844098
#define CFG_CPU_THROT		15

/* SPI */
#define SPI0_BASE	0xbfe80000

#define SPCR		0
#define SPSR		1
#define TxFIFO		2
#define RxFIFO		2
#define SPER		3
#define SFC_PARAM	4
#define SFC_SOFTCS	5
#define SFC_TIMING	6

/* SDRAM */
#define SD_CONFIG	0xbfd00000
#define SD_CONFIGHI	0x414
#define SD_CONFIGLO	0x410
#define CONFIG_VALID	0x200

#define CFG_SDCONFIGHI	0x00000028
#define CFG_SDCONFIGLO	0x028A924A

	.set noreorder

/*	PLL: 264MHz	CPU: 132MHz	SDRAM: 66MHz	*/
LEAF(ls1c300_pll_init)
#if !CONFIG_IS_ENABLED(SKIP_LOWLEVEL_INIT)
	li	t0, NAND_BASE
	li	t1, CFG_START_FREQ
	li	t2, CFG_CLK_DIV_PARAM
	li	t3, CFG_CPU_THROT

	sw	t3, CPU_THROT (t0)
	sw	t2, CLK_DIV_PARAM (t0)
	sw	t1, START_FREQ (t0)
	  nop

	ori	t2, CPU_SEL
	sw	t2, CLK_DIV_PARAM (t0)

#endif
	li	v0, 264000000
	jr	ra
	nop
END(ls1c300_pll_init)

/* SPI: Dual IO@33MHz */
LEAF(ls1c300_spi_init)
#if !CONFIG_IS_ENABLED(SKIP_LOWLEVEL_INIT)
	li	t0, SPI0_BASE
	li	t1, 0x07
	li	t2, 0x05
	sb	t1, SFC_PARAM (t0)
	sb	t2, SFC_TIMING (t0)
#endif
	jr	ra
	  nop
END(ls1c300_spi_init)

/* SDRAM: 66MHz */
// 8M x 16Bit x 4 Banks */
// Organization | Row Address | Column Address
// 32Mx16       | A0~A12      | A0-A9

LEAF(ls1c300_sdram_init)
#if !CONFIG_IS_ENABLED(SKIP_LOWLEVEL_INIT)
	li	t0, SD_CONFIG
	li	t1, CFG_SDCONFIGLO
	li	t2, CFG_SDCONFIGHI

	sw	t1, SD_CONFIGLO (t0)
	sw	t2, SD_CONFIGHI (t0)
	nop

	sw	t1, SD_CONFIGLO (t0)
	sw	t2, SD_CONFIGHI (t0)
	nop

	ori	t2, CONFIG_VALID
	sw	t1, SD_CONFIGLO (t0)
	sw	t2, SD_CONFIGHI (t0)
#endif
	li	v0, SZ_64M
	jr	ra
	 nop
END(ls1c300_sdram_init)

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
