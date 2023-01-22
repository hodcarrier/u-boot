/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2013 Regents of the University of California
 */

#include <linux/linkage.h>
#include <asm/asm.h>

#include <asm/asm-la.h>

/* void *memcpy(void *, const void *, size_t) */
ENTRY(__memcpy)
WEAK(memcpy)
	/* Save for return value */
	mv	t6, a0

	/*
	 * Register allocation for code below:
	 * a0 - start of uncopied dst
	 * a1 - start of uncopied src
	 * t0 - end of uncopied dst
	 */
	add	t0, a0, a2

	/*
	 * Use bytewise copy if too small.
	 *
	 * This threshold must be at least 2*SZREG to ensure at least one
	 * wordwise copy is performed. It is chosen to be 16 because it will
	 * save at least 7 iterations of bytewise copy, which pays off the
	 * fixed overhead.
	 */
	li	a3, 16
	bltu	a2, a3, .Lbyte_copy_tail

	/*
	 * Bytewise copy first to align a0 to word boundary.
	 */
	addi	a2, a0, SZREG-1
	li	t1, ~(SZREG-1)
	and	a2, a2, t1
	beq	a0, a2, 2f
1:
	lb	a5, a1, 0
	addi	a1, a1, 1
	sb	a5, a0, 0
	addi	a0, a0, 1
	bne	a0, a2, 1b
2:

	/*
	 * Now a0 is word-aligned. If a1 is also word aligned, we could perform
	 * aligned word-wise copy. Otherwise we need to perform misaligned
	 * word-wise copy.
	 */
	andi	a3, a1, SZREG-1
	bnez	a3, .Lmisaligned_word_copy

	/* Unrolled wordwise copy */
	addi	t0, t0, -(16*SZREG-1)
	bgeu	a0, t0, 2f
1:
	REG_L	a2, a1, 0
	REG_L	a3, a1, SZREG
	REG_L	a4, a1, SZREG*2
	REG_L	a5, a1, SZREG*3
	REG_L	a6, a1, SZREG*4
	REG_L	a7, a1, SZREG*5
	REG_L	t1, a1, SZREG*6
	REG_L	t2, a1, SZREG*7
	REG_L	t3, a1, SZREG*8
	REG_L	t4, a1, SZREG*9
	REG_L	t5, a1, SZREG*10
	REG_S	a2, a0, 0
	REG_S	a3, a0, SZREG
	REG_S	a4, a0, SZREG*2
	REG_S	a5, a0, SZREG*3
	REG_S	a6, a0, SZREG*4
	REG_S	a7, a0, SZREG*5
	REG_S	t1, a0, SZREG*6
	REG_S	t2, a0, SZREG*7
	REG_S	t3, a0, SZREG*8
	REG_S	t4, a0, SZREG*9
	REG_S	t5, a0, SZREG*10
	REG_L	a2, a1, SZREG*11
	REG_L	a3, a1, SZREG*12
	REG_L	a4, a1, SZREG*13
	REG_L	a5, a1, SZREG*14
	REG_L	a6, a1, SZREG*15
	addi	a1, a1, SZREG*16
	REG_S	a2, a0, SZREG*11
	REG_S	a3, a0, SZREG*12
	REG_S	a4, a0, SZREG*13
	REG_S	a5, a0, SZREG*14
	REG_S	a6, a0, SZREG*15
	addi	a0, a0, SZREG*16
	bltu	a0, t0, 1b
2:
	/* Post-loop increment by 16*SZREG-1 and pre-loop decrement by SZREG-1 */
	addi	t0, t0, 15*SZREG

	/* Wordwise copy */
	bgeu	a0, t0, 2f
1:
	REG_L	a5, a1, 0
	addi	a1, a1, SZREG
	REG_S	a5, a0, 0
	addi	a0, a0, SZREG
	bltu	a0, t0, 1b
2:
	addi	t0, t0, SZREG-1

.Lbyte_copy_tail:
	/*
	 * Bytewise copy anything left.
	 */
	beq	a0, t0, 2f
1:
	lb	a5, a1, 0
	addi	a1, a1, 1
	sb	a5, a0, 0
	addi	a0, a0, 1
	bne	a0, t0, 1b
2:

	mv	a0, t6
	ret

.Lmisaligned_word_copy:
	/*
	 * Misaligned word-wise copy.
	 * For misaligned copy we still perform word-wise copy, but we need to
	 * use the value fetched from the previous iteration and do some shifts.
	 * This is safe because we wouldn't access more words than necessary.
	 */

	/* Calculate shifts */
	slli	t3, a3, 3
	sub	t4, x0, t3 /* negate is okay as shift will only look at LSBs */

	/* Load the initial value and align a1 */
	li	t1, ~(SZREG-1)
	and	a1, a1, t1
	REG_L	a5, a1, 0

	addi	t0, t0, -(SZREG-1)
	/* At least one iteration will be executed here, no check */
1:
	srl	a4, a5, t3
	REG_L	a5, a1, SZREG
	addi	a1, a1, SZREG
	sll	a2, a5, t4
	or	a2, a2, a4
	REG_S	a2, a0, 0
	addi	a0, a0, SZREG
	bltu	a0, t0, 1b

	/* Update pointers to correct value */
	addi	t0, t0, SZREG-1
	add	a1, a1, a3

	j	.Lbyte_copy_tail
END(__memcpy)
