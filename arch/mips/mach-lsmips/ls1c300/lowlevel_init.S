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
#define NAND_BASE	0xbfe78000
#define START_FREQ	0xbfe78030
#define CLK_DIV_PARAM	0xbfe78034

/* SPI */
#define SPI0_BASE	0xbfe80000

/* SDRAM */
#define	SD_CONFIG	0xbfd00000
#define	SD_CONFIGL	0xbfd00410
#define	SD_CONFIGH	0xbfd00414

	.set noreorder

/*	PLL: 264MHz	CPU: 132MHz	SDRAM: 66MHz	*/
LEAF(ls1c300_pll_init)
#ifndef CONFIG_SKIP_LOWLEVEL_INIT
	li	t0, NAND_BASE
	li	t2, 555844098
	sw	t2, 0x34 (t0)

	li	t1, 2147494924
	sw	t1, 0x30 (t0)

	ori	t2, 1
	sw	t2, 0x34 (t0)
#endif
/* TODO: recalc rate to v0 */
	li	v0, 132000000
	jr	ra
	  nop
END(ls1c300_pll_init)

/* SPI: Dual IO@33MHz */
LEAF(ls1c300_spi_init)
#ifndef CONFIG_SKIP_LOWLEVEL_INIT
	li	t0, SPI0_BASE
	li	t1, 0x07
	li	t2, 0x05
	sb	t1, 0x4(t0)
	sb	t1, 0x6(t0)
#endif
	jr	ra
	  nop
END(ls1c300_spi_init)

/* SDRAM: 66MHz */
// 8M x 16Bit x 4 Banks */
// Organization | Row Address | Column Address
// 32Mx16       | A0~A12      | A0-A9

// 128Mx4       | A0~A12      | A0-A9, A11, A12
// 64Mx8        | A0~A12      | A0-A9, A11
// 32Mx16       | A0~A12      | A0-A9

LEAF(ls1c300_sdram_init)
#ifndef CONFIG_SKIP_LOWLEVEL_INIT
	li	t0, SD_CONFIG
	li	t1, 0x028A924A
	li	t2, 0x00000028
sdram_cfg:
	sw	t1, 0x410(t0)
	sw	t2, 0x414(t0)
	nop
	sw	t1, 0x410(t0)
	sw	t2, 0x414(t0)
	ori	t2, 0x200
	sw	t1, 0x410(t0)
	sw	t2, 0x414(t0)
#endif /* CONFIG_SKIP_LOWLEVEL_INIT */
	li	v0, SZ_64M
	jr	ra
	 nop
END(ls1c300_sdram_init)


/*
 *	SDRAM@132MHz, CPU@297MHz, SPI@33MHz
 *	SDRAM@100MHz, CPU@300MHz, SPI@50MHz
 *	SDRAM@66MHz, CPU@132MHz, SPI@33MHz  <--
 */

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
