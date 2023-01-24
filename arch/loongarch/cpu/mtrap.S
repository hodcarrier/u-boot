/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * M-mode Trap Handler Code for RISC-V Core
 *
 * Copyright (c) 2017 Microsemi Corporation.
 * Copyright (c) 2017 Padmarao Begari <Padmarao.Begari@microsemi.com>
 *
 * Copyright (C) 2017 Andes Technology Corporation
 * Rick Chen, Andes Technology Corporation <rick@andestech.com>
 *
 * Copyright (C) 2018, Bin Meng <bmeng.cn@gmail.com>
 */

#include <common.h>
#include <asm/encoding.h>
#include <asm/regdef.h>
#include <asm/asm-la.h>

#ifdef CONFIG_32BIT
#define LREG		lw
#define SREG		sw
#define REGBYTES	4
#else
#define LREG		ld
#define SREG		sd
#define REGBYTES	8
#endif

	.text

	/* trap entry */
	.align 12
	.global trap_entry
trap_entry:
	addi sp, sp, -32 * REGBYTES
	SREG ra,  sp,  1 * REGBYTES
	SREG tp,  sp,  2 * REGBYTES
	SREG sp,  sp,  3 * REGBYTES
	SREG a0,  sp,  4 * REGBYTES
	SREG a1,  sp,  5 * REGBYTES
	SREG a2,  sp,  6 * REGBYTES
	SREG a3,  sp,  7 * REGBYTES
	SREG a4,  sp,  8 * REGBYTES
	SREG a5,  sp,  9 * REGBYTES
	SREG a6,  sp, 10 * REGBYTES
	SREG a7,  sp, 11 * REGBYTES
	SREG t0,  sp, 12 * REGBYTES
	SREG t1,  sp, 13 * REGBYTES
	SREG t2,  sp, 14 * REGBYTES
	SREG t3,  sp, 15 * REGBYTES
	SREG t4,  sp, 16 * REGBYTES
	SREG t5,  sp, 17 * REGBYTES
	SREG t6,  sp, 18 * REGBYTES
	SREG t7,  sp, 19 * REGBYTES
	SREG t8,  sp, 20 * REGBYTES
	SREG u0,  sp, 21 * REGBYTES
	SREG fp,  sp, 22 * REGBYTES
	SREG s0,  sp, 23 * REGBYTES
	SREG s1,  sp, 24 * REGBYTES
	SREG s2,  sp, 25 * REGBYTES
	SREG s3,  sp, 26 * REGBYTES
	SREG s4,  sp, 27 * REGBYTES
	SREG s5,  sp, 28 * REGBYTES
	SREG s6,  sp, 29 * REGBYTES
	SREG s7,  sp, 30 * REGBYTES
	SREG s8,  sp, 31 * REGBYTES
	csrrd a0, 0x05	// ESTAT MODE_PREFIX(cause)
	csrrd a1, 0x07	// BADV  MODE_PREFIX(epc)
	csrrd a2, 0x42	// TVAL  MODE_PREFIX(tval)
	move a3, sp
	bl handle_trap
	csrwr a0, 7	// MODE_PREFIX(epc), a0
	LREG ra, sp,  1 * REGBYTES
	LREG tp, sp,  2 * REGBYTES
	LREG sp, sp,  3 * REGBYTES
	LREG a0, sp,  4 * REGBYTES
	LREG a1, sp,  5 * REGBYTES
	LREG a2, sp,  6 * REGBYTES
	LREG a3, sp,  7 * REGBYTES
	LREG a4, sp,  8 * REGBYTES
	LREG a5, sp,  9 * REGBYTES
	LREG a6, sp, 10 * REGBYTES
	LREG a7, sp, 11 * REGBYTES
	LREG t0, sp, 12 * REGBYTES
	LREG t1, sp, 13 * REGBYTES
	LREG t2, sp, 14 * REGBYTES
	LREG t3, sp, 15 * REGBYTES
	LREG t4, sp, 16 * REGBYTES
	LREG t5, sp, 17 * REGBYTES
	LREG t6, sp, 18 * REGBYTES
	LREG t7, sp, 19 * REGBYTES
	LREG t8, sp, 20 * REGBYTES
	LREG u0, sp, 21 * REGBYTES
	LREG fp, sp, 22 * REGBYTES
	LREG s0, sp, 23 * REGBYTES
	LREG s1, sp, 24 * REGBYTES
	LREG s2, sp, 25 * REGBYTES
	LREG s3, sp, 26 * REGBYTES
	LREG s4, sp, 27 * REGBYTES
	LREG s5, sp, 28 * REGBYTES
	LREG s6, sp, 29 * REGBYTES
	LREG s7, sp, 30 * REGBYTES
	LREG s8, sp, 31 * REGBYTES
	LREG sp, sp,  3 * REGBYTES
	addi sp, sp, 32 * REGBYTES
	ertn
