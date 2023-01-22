/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) 2018 Alexander Graf <agraf@suse.de>
 */

#include <config.h>
#include <linux/linkage.h>
#include <asm/asm-la.h>

#ifdef CONFIG_ARCH_LA64I
#define STORE_IDX(reg, idx)	sd reg, a0, idx*8
#define LOAD_IDX(reg, idx)	ld reg, a0, idx*8
#else
#define STORE_IDX(reg, idx)	sw reg, a0, idx*4
#define LOAD_IDX(reg, idx)	lw reg, a0, idx*4
#endif

.pushsection .text.setjmp, "ax"
ENTRY(setjmp)
	/* Preserve all callee-saved registers and the SP */
	STORE_IDX(fp, 0)
	STORE_IDX(s1, 1)
	STORE_IDX(s2, 2)
	STORE_IDX(s3, 3)
	STORE_IDX(s4, 4)
	STORE_IDX(s5, 5)
	STORE_IDX(s6, 6)
	STORE_IDX(s7, 7)
	STORE_IDX(s8, 8)
	STORE_IDX(s0, 9)
	STORE_IDX(u0, 10)
	STORE_IDX(tp, 11)
	STORE_IDX(ra, 12)
	STORE_IDX(sp, 13)
	li  a0, 0
	ret
ENDPROC(setjmp)
.popsection

.pushsection .text.longjmp, "ax"
ENTRY(longjmp)
	LOAD_IDX(fp, 0)
	LOAD_IDX(s1, 1)
	LOAD_IDX(s2, 2)
	LOAD_IDX(s3, 3)
	LOAD_IDX(s4, 4)
	LOAD_IDX(s5, 5)
	LOAD_IDX(s6, 6)
	LOAD_IDX(s7, 7)
	LOAD_IDX(s8, 8)
	LOAD_IDX(s0, 9)
	LOAD_IDX(u0, 10)
	LOAD_IDX(tp, 11)
	LOAD_IDX(ra, 12)
	LOAD_IDX(sp, 13)

	/* Move the return value in place, but return 1 if passed 0. */
	seqz a0, a1
	add a0, a0, a1
	ret
ENDPROC(longjmp)
.popsection
