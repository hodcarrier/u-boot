/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2015 Regents of the University of California
 */

#ifndef _ASM_LOONGARCH_ASM_H
#define _ASM_LOONGARCH_ASM_H

#ifdef __ASSEMBLY__
#define __ASM_STR(x)	x
#else
#define __ASM_STR(x)	#x
#endif

#if __loongarch_grlen == 64
#define __REG_SEL(a, b)	__ASM_STR(a)
#elif __loongarch_grlen == 32
#define __REG_SEL(a, b)	__ASM_STR(b)
#else
#error "Unexpected __loongarch_grlen"
#endif

#define REG_L		__REG_SEL(ld, lw)
#define REG_S		__REG_SEL(sd, sw)
#define SZREG		__REG_SEL(8, 4)
#define LGREG		__REG_SEL(3, 2)

#if __SIZEOF_POINTER__ == 8
#ifdef __ASSEMBLY__
#define LOONGARCH_PTR		.dword
#define LOONGARCH_SZPTR		8
#define LOONGARCH_LGPTR		3
#else
#define LOONGARCH_PTR		".dword"
#define LOONGARCH_SZPTR		"8"
#define LOONGARCH_LGPTR		"3"
#endif
#elif __SIZEOF_POINTER__ == 4
#ifdef __ASSEMBLY__
#define LOONGARCH_PTR		.word
#define LOONGARCH_SZPTR		4
#define LOONGARCH_LGPTR		2
#else
#define LOONGARCH_PTR		".word"
#define LOONGARCH_SZPTR		"4"
#define LOONGARCH_LGPTR		"2"
#endif
#else
#error "Unexpected __SIZEOF_POINTER__"
#endif

#if (__SIZEOF_INT__ == 4)
#define LOONGARCH_INT		__ASM_STR(.word)
#define LOONGARCH_SZINT		__ASM_STR(4)
#define LOONGARCH_LGINT		__ASM_STR(2)
#else
#error "Unexpected __SIZEOF_INT__"
#endif

#if (__SIZEOF_SHORT__ == 2)
#define LOONGARCH_SHORT		__ASM_STR(.half)
#define LOONGARCH_SZSHORT	__ASM_STR(2)
#define LOONGARCH_LGSHORT	__ASM_STR(1)
#else
#error "Unexpected __SIZEOF_SHORT__"
#endif

#endif /* _ASM_LOONGARCH_ASM_H */
