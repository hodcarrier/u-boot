// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2020 MediaTek Inc.
 *
 * Author:  Gao Weijie <weijie.gao@mediatek.com>
 *
 * Copyright (C) 2020-2022 Du Huanpeng <dhu@hodcarrier.org>
 */

#include <mach/serial.h>
#include <linux/kernel.h>

struct uart_pin_config {
	char port;
	char af;
	char rx;
	char tx;
};

struct uart_pin_config con[] = {
#if CONFIG_CONS_INDEX == 0
	{ 0, 2, 74, 75 },
	{ 0, 3, 23, 24 },
	{ 0, 3, 99, 100 },

#elif CONFIG_CONS_INDEX == 1
	{ 1, 1, 17, 18 },
	{ 1, 1, 101, 102 },
	{ 1, 2, 40, 41 },
	{ 1, 4, 2, 3 },

#elif CONFIG_CONS_INDEX == 2
	{ 2, 2, 36, 37 },
	{ 2, 2, 42, 43 },
	{ 2, 3, 27, 28 },
	{ 2, 3, 103, 104 },
	{ 2, 4, 4, 5 },

#elif CONFIG_CONS_INDEX == 3
	{ 3, 2, 17, 18 },
	{ 3, 2, 33, 34 },
	{ 3, 2, 44, 45 },
	{ 3, 4, 0, 1 },

#elif CONFIG_CONS_INDEX == 4
	{ 4, 5, 23, 24 },
	{ 4, 5, 58, 59 },
	{ 4, 5, 80, 79 },

#elif CONFIG_CONS_INDEX == 5
	{ 5, 5, 25, 26 },
	{ 5, 5, 60, 61 },
	{ 5, 5, 81, 78 },

#elif CONFIG_CONS_INDEX == 6
	{ 6, 5, 27, 46 },
	{ 6, 5, 62, 63 },

#elif CONFIG_CONS_INDEX == 7
	{ 7, 5, 57, 56 },
	{ 7, 5, 64, 65 },
	{ 7, 5, 87, 88 },

#elif CONFIG_CONS_INDEX == 8
	{ 8, 5, 55, 54 },
	{ 8, 5, 66, 67 },
	{ 8, 5, 89, 90 },

#elif CONFIG_CONS_INDEX == 9
	{ 9, 5, 53, 52 },
	{ 9, 5, 68, 69 },
	{ 9, 5, 85, 86 },

#elif CONFIG_CONS_INDEX == 10
	{ 10, 5, 51, 50 },
	{ 10, 5, 70, 71 },
	{ 10, 5, 84, 82 },

#elif CONFIG_CONS_INDEX == 11
	{ 11, 5, 49, 48 },
	{ 11, 5, 72, 73 },
#endif /* CONFIG_CONS_INDEX */
};

#define UART2_RX	36
#define UART2_TX	37
#define AFUNC		2

void lsmips_spl_serial_init(void)
{
#if defined(CONFIG_SPL_SERIAL)
	int pin_rx, pin_tx;
	int afunc;

	if (CONFIG_CONS_PIN < ARRAY_SIZE(con)) {
		pin_rx = con[CONFIG_CONS_PIN].rx;
		pin_tx = con[CONFIG_CONS_PIN].tx;
		afunc = con[CONFIG_CONS_PIN].af;
	} else {
		pin_rx = UART2_RX;
		pin_tx = UART2_TX;
		afunc = AFUNC;
	}

	gpio_set_alternate(pin_rx, afunc);
	gpio_set_alternate(pin_tx, afunc);
#endif /* CONFIG_SPL_SERIAL */
	return;
}
