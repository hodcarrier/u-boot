/* SPDX-License-Identifier: GPL-2.0 */
/*
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

/* sdram control 64 bit register */
#define SD_CONFIG	0xbfd00000
#define SD_CONFIGHI	0x414
#define SD_CONFIGLO	0x410

#define CONFIG_VALID	41
#define HANG_UP		40
#define DEF_SEL		39
#define TWR		37
#define TREF		25
#define TRAS		21
#define TRFC		17
#define TRP		14
#define TCL		11
#define TRCD		8

#define SD_BIT		6
  #define SD_8BIT       (0<<SD_BIT)
  #define SD_16BIT      (1<<SD_BIT)
  #define SD_32BIT      (2<<SD_BIT)
#define SD_CSIZE	3
  #define SD_CSIZE_512	(0<<SD_CSIZE)
  #define SD_CSIZE_1K	(1<<SD_CSIZE)
  #define SD_CSIZE_2K	(2<<SD_CSIZE)
  #define SD_CSIZE_4K	(3<<SD_CSIZE)
  #define SD_CSIZE_256	(7<<SD_CSIZE)
#define SD_RSIZE	0
  #define SD_RSIZE_2K	(0<<SD_RSIZE)
  #define SD_RSIZE_4K	(1<<SD_RSIZE)
  #define SD_RSIZE_8K	(2<<SD_RSIZE)
  #define SD_RSIZE_16K	(3<<SD_RSIZE)

#define SD_CFG_1(tWR, tREF, tRAS, tRFC, tRP, tCL, tRCD) \
	((tWR<<TWR)|(tREF<<TREF)|(tRAS<<TRAS)|(tRFC<<TRFC)|(tRP<<TRP)|(tCL<<TCL)|(tRCD<<TRCD))
#define CFG_SD_0(b, c, r) \
	((b<<SD_BIT)|(c<<SD_CSIZE)|(r<<SD_RSIZE))
/*
 * recommended values by ls1c300 user manual,
 * tweak to fit your board.
 */
#define SD_CONFIG_133MHz	SD_CFG_1(2, 0x818, 6, 8, 3, 3, 3)
#define SD_CONFIG_100MHz	SD_CFG_1(2, 0x620, 5, 6, 2, 3, 2)
#define SD_CONFIG_75MHz		SD_CFG_1(1, 0x494, 4, 5, 2, 2, 2)
#define SD_CONFIG_33MHz		SD_CFG_1(1, 0x204, 2, 2, 1, 2, 1)

#define SD_CONFIG_66MHz		SD_CFG_1(1, 0x401, 4, 4, 2, 2, 2)

#define SD_CONFIG64	(SD_CONFIG_66MHz | SD_16BIT | SD_CSIZE_1K | SD_RSIZE_8K)
#define CFG_SDCONFIGHI	(SD_CONFIG64 /(1<<32))
#define CFG_SDCONFIGLO	(SD_CONFIG64 %(1<<32))

	.set noreorder
/*
 * Loongson ls1c300 SoC do not have onchip sram for initial stack,
 * initialize the external sdram on reset as early as possiable.
 */
LEAF(ls1c300_sdram_init)
#if !CONFIG_IS_ENABLED(SKIP_LOWLEVEL_INIT)
	li	t0, SD_CONFIG
	li	t1, CFG_SDCONFIGLO
	li	t2, CFG_SDCONFIGHI

/* store twice as the hardware manual required. */
	sw	t1, SD_CONFIGLO (t0)
	sw	t2, SD_CONFIGHI (t0)
	sync

	sw	t1, SD_CONFIGLO (t0)
	sw	t2, SD_CONFIGHI (t0)
	sync

	ori	t2, 1<<(CONFIG_VALID-32)
	sw	t1, SD_CONFIGLO (t0)
	sw	t2, SD_CONFIGHI (t0)
	sync
#endif
	li	v0, SZ_64M
	jr	ra
	 nop
END(ls1c300_sdram_init)


