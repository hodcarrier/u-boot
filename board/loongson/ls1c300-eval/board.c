// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2020 MediaTek Inc. All Rights Reserved.
 *
 * Author: Gao Weijie <weijie.gao@mediatek.com>
 *
 * Copyright (C) 2020-2022 Du Huanpeng <dhu@hodcarrier.org>
 */

#include <common.h>


#ifdef CONFIG_DEBUG_UART_BOARD_INIT

/* 引脚复用，波特率设置在公共代码 */

void board_debug_uart_init(void)
{
	volatile int *p = (void *)(0xbfd011c4+0x10);

	/* UART2 */
//	li		a0, 0xbfd011c4	/* 第一复用寄存器 0 */
//	lw		a1, 0x10(a0)
//	ori		a1, 0x30
//	sw		a1, 0x10(a0)

	p[0] |= 0x30;

	return ;
}
#endif


#ifdef initserial
// LEAF(initserial)
/* -------------------------------------- */
/* -------------------------------------- */
	la	v0, CONFIG_DEBUG_UART_BASE
1:
	li	v1, (FIFO_ENABLE|FIFO_RCV_RST|FIFO_XMT_RST|FIFO_TRIGGER_4)
	sb	v1, NSREG(NS16550_FIFO)(v0)
	li	v1, CFCR_DLAB
	sb	v1, NSREG(NS16550_CFCR)(v0)

	li	v1, CONFIG_DEBUG_UART_CLOCK
	li	a1, 16*CONS_BAUD
	divu	v1, v1, a1
/* ------------------------------ */
	sb	v1, NSREG(NS16550_DATA)(v0)
	srl	v1, 8
	sb	v1, NSREG(NS16550_IER)(v0)
	li	v1, CFCR_8BITS
	sb	v1, NSREG(NS16550_CFCR)(v0)
	li	v1, MCR_DTR|MCR_RTS
	sb	v1, NSREG(NS16550_MCR)(v0)
	li	v1, 0x0
	sb	v1, NSREG(NS16550_IER)(v0)

	#disable all interrupt
	li  v1, 0x0
	sb  v1, NSREG(NS16550_IER)(v0)
// END(initserial)
/* ------------------------------ */

#endif
