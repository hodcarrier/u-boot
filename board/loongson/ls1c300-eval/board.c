// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2020-2022 Du Huanpeng <dhu@hodcarrier.org>
 */

#include <common.h>
#include <mach/serial.h>

#ifdef CONFIG_DEBUG_UART_BOARD_INIT

#define UART2_RX	36
#define UART2_TX	37
#define AFUNC		2

void board_debug_uart_init(void)
{
	gpio_set_alternate(UART2_TX, AFUNC);
	gpio_set_alternate(UART2_RX, AFUNC);
}
#endif
