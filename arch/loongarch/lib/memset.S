/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2013 Regents of the University of California
 */


#include <linux/linkage.h>
#include <asm/asm.h>
#include <asm/asm-la.h>


/* void *memset(void *, int, size_t) */
ENTRY(__memset)
WEAK(memset)
	move t0, a0  /* Preserve return value */

	/* Defer to byte-oriented fill for small sizes */
	sltiu a3, a2, 16
	bnez a3, 4f

	/*
	 * Round to nearest XLEN-aligned address
	 * greater than or equal to start address
	 */
	addi a3, t0, SZREG-1

	li t1, ~(SZREG-1)
	and a3, a3, t1
	beq a3, t0, 2f  /* Skip if already aligned */
	/* Handle initial misalignment */
	sub a4, a3, t0
1:
	sb a1, t0, 0
	addi t0, t0, 1
	bltu t0, a3, 1b
	sub a2, a2, a4  /* Update count */

2: /* Duff's device with 32 XLEN stores per iteration */
	/* Broadcast value into all bytes */
	andi a1, a1, 0xff
	slli a3, a1, 8
	or a1, a3, a1
	slli a3, a1, 16
	or a1, a3, a1
#ifdef CONFIG_64BIT
	slli a3, a1, 32
	or a1, a3, a1
#endif

	/* Calculate end address */
	li t1, ~(SZREG-1)
	and a4, a2, t1
	add a3, t0, a4

	andi a4, a4, 31*SZREG  /* Calculate remainder */
	beqz a4, 3f            /* Shortcut if no remainder */
	neg a4, a4
	addi a4, a4, 32*SZREG  /* Calculate initial offset */

	/* Adjust start address with offset */
	sub t0, t0, a4

	/* Jump into loop body */
	/* Assumes 32-bit instruction lengths */
	la a5, 3f
#ifdef CONFIG_64BIT
	srli a4, a4, 1
#endif
	add a5, a5, a4
	jr a5
3:
	REG_S a1, t0, SZREG*0
	REG_S a1, t0, SZREG*1
	REG_S a1, t0, SZREG*2
	REG_S a1, t0, SZREG*3
	REG_S a1, t0, SZREG*4
	REG_S a1, t0, SZREG*5
	REG_S a1, t0, SZREG*6
	REG_S a1, t0, SZREG*7
	REG_S a1, t0, SZREG*8
	REG_S a1, t0, SZREG*9
	REG_S a1, t0, SZREG*10
	REG_S a1, t0, SZREG*11
	REG_S a1, t0, SZREG*12
	REG_S a1, t0, SZREG*13
	REG_S a1, t0, SZREG*14
	REG_S a1, t0, SZREG*15
	REG_S a1, t0, SZREG*16
	REG_S a1, t0, SZREG*17
	REG_S a1, t0, SZREG*18
	REG_S a1, t0, SZREG*19
	REG_S a1, t0, SZREG*20
	REG_S a1, t0, SZREG*21
	REG_S a1, t0, SZREG*22
	REG_S a1, t0, SZREG*23
	REG_S a1, t0, SZREG*24
	REG_S a1, t0, SZREG*25
	REG_S a1, t0, SZREG*26
	REG_S a1, t0, SZREG*27
	REG_S a1, t0, SZREG*28
	REG_S a1, t0, SZREG*29
	REG_S a1, t0, SZREG*30
	REG_S a1, t0, SZREG*31
	addi  t0, t0, SZREG*32
	bltu  t0, a3, 3b
	andi  a2, a2, SZREG-1  /* Update count */

4:
	/* Handle trailing misalignment */
	beqz a2, 6f
	add a3, t0, a2
5:
	sb a1, t0, 0
	addi t0, t0, 1
	bltu t0, a3, 5b
6:
	ret
END(__memset)
