// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2022 Du Huanpeng <dhu@hodcarrier.org>
 */

#include <linux/errno.h>
#include <linux/bitops.h>
#include <asm/bitops.h>
#include <asm/io.h>

#define CBUS_FIRST0	0xbfd011c0
#define CBUS_SECOND0	0xbfd011d0
#define CBUS_THIRD0	0xbfd011e0
#define CBUS_FOURTHT0	0xbfd011f0
#define CBUS_FIFTHT0	0xbfd01200

#define CBUS_FIRST1	0xbfd011c4
#define CBUS_SECOND1	0xbfd011d4
#define CBUS_THIRD1	0xbfd011e4
#define CBUS_FOURTHT1	0xbfd011f4
#define CBUS_FIFTHT1	0xbfd01204

#define CBUS_FIRST2	0xbfd011c8
#define CBUS_SECOND2	0xbfd011d8
#define CBUS_THIRD2	0xbfd011e8
#define CBUS_FOURTHT2	0xbfd011f8
#define CBUS_FIFTHT2	0xbfd01208

#define CBUS_FIRST3	0xbfd011cc
#define CBUS_SECOND3	0xbfd011dc
#define CBUS_THIRD3	0xbfd011ec
#define CBUS_FOURTHT3	0xbfd011fc
#define CBUS_FIFTHT3	0xbfd0120c

int gpio_set_alternate(int gpio, int func)
{
	volatile void __iomem *addr;
	int i;

	if (gpio < 0 || gpio > 104)
		return -ENODEV;
	if (func < 0)
		return -EINVAL;

	if (func) {
		i = func - 1;
		addr = (void *)CBUS_FIRST0 + i * 16;
		set_bit(gpio, addr);
	} else {
		/* GPIO, clear CBUS 1 ~ 5 */
		i = 5;
	}

	while (i--) {
		addr = (void *)CBUS_FIRST0 + 16 * i;
		clear_bit(gpio, addr);
	}

	return 0;
}
