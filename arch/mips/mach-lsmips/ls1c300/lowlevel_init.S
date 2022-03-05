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
#include "ls1c300.h"
#include <linux/sizes.h>

/* 寄存器地址 */

#define START_FREQ	0xbfe78030
#define CLK_DIV_PARAM	0xbfe78034

/* SDRAM 参数寄存器 */
#define	SD_CONFIGL	0xbfd00410
#define	SD_CONFIGH	0xbfd00414

/* Set temporary stack address range */
#ifndef CONFIG_SYS_INIT_SP_ADDR
#define CONFIG_SYS_INIT_SP_ADDR	(CONFIG_SYS_SDRAM_BASE + \
				CONFIG_SYS_INIT_SP_OFFSET)
#endif

#define CACHE_STACK_SIZE	0x4000
#define CACHE_STACK_BASE	(CONFIG_SYS_INIT_SP_ADDR - CACHE_STACK_SIZE)

	.set noreorder

/*	PLL: 528MHz	CPU: 264MHz	SDRAM: 132MHz	*/
LEAF(ls1c300_pll_init_264mhz)
	li	t0, 0xbfe78030
	li	t2, 555844098
	sw	t2, 4 (t0)

	li	t1, 2147506188
	sw	t1, 0 (t0)
	nop

	ori	t2, 1
	sw	t2, 4 (t0)

	li	v0, 264000000
	jr	ra
	  nop
END(ls1c300_pll_init_264mhz)

/* SPI: Dual IO@33MHz */
LEAF(ls1c300_spi_init)
	li	t0, 0xbfe80000
	li	t1, 0x07
	li	t2, 0x05
	sb	t1, 0x4(t0)
	sb	t1, 0x6(t0)
	jr	ra
	  nop
END(ls1c300_spi_init)

/*	PLL: 264MHz	CPU: 132MHz	SDRAM: 66MHz	*/
LEAF(ls1c300_pll_init)
	li	t0, 0xbfe78030
	li	t2, 555844098
	sw	t2,	4 (t0)

	li	t1, 2147494924
	sw	t1, 0 (t0)

	ori	t2, 1
	sw	t2, 4 (t0)

	li	v0, 132000000
	jr	ra
	  nop
END(ls1c300_pll_init)


/* SDRAM: 66MHz */
// 8M x 16Bit x 4 Banks */
// Organization | Row Address | Column Address
// 32Mx16       | A0~A12      | A0-A9

// 128Mx4       | A0~A12      | A0-A9, A11, A12
// 64Mx8        | A0~A12      | A0-A9, A11
// 32Mx16       | A0~A12      | A0-A9

LEAF(ls1c300_sdram_init)
#ifndef CONFIG_SKIP_LOWLEVEL_INIT
	li	t0, 0xbfd00000
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

//#if CONFIG_IS_ENABLED(INIT_STACK_WITHOUT_MALLOC_F)
// 这里还没有配置 gd 恐怕不可用，稍后再厘清。
//	/* Set malloc base */
//	li	t0, (CONFIG_SYS_INIT_SP_ADDR + 15) & (~15)
//	PTR_S	t0, GD_MALLOC_BASE(k0)	# gd->malloc_base offset
//
//#endif

	move	ra, s0
	jr	ra
	 nop
	END(lowlevel_init)
