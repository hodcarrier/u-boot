#ifndef __ASM_LA_H__
#define __ASM_LA_H__

#define sltiu   SLTUI
#define addi	ADDI.d
#define sd	ST.d
#define sb	ST.b
#define ld	LD.d
#define lb	LD.b
#define li	LI.d
#define sub	SUB.d
#define slli	SLLI.d
#define add	ADD.d
#define ret	JR ra
#define srli	SRLI.d
#define sll	SLL.d
#define srl	SRL.d
#define j	B
#define andi	ANDI
#define mv	MOVE
#define sll	SLL.d
#define jalr	JR
#define jal	BL
#define mul	mul.d

/* broken instructions */
#define wfi	nop
#define tail    B

#define x0	zero
#define u0	$r21

.macro seqz a0 a1
	SLTUI \a0, \a1, 1
.endm

.macro neg a0 a1
	sub	\a0, $zero, \a1
.endm

#include <asm/regdef.h>

#endif

