// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2022 Du Huanpeng <dhu@hodcarrier.org>
 */

#include <linux/errno.h>
#include <linux/bitops.h>
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
	int i;
	unsigned long bank, mask;
	volatile void __iomem *addr;

	if(gpio <0 || gpio > 104) return -ENODEV;
	if(func <1 || func >5) return -EINVAL;

	addr = (void *)CBUS_FIRST0;

	bank = gpio / 32;
	mask = BIT(gpio % 32);
	for (i = 0; i < func - 1; i++) {
		clrbits_32(addr + 0x10 * i + 4 * bank, mask);
	}

	setbits_32(addr + 0x10 * i + 4 * bank, mask);

	return 0;
}

