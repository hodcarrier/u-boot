// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2020, Sean Anderson <seanga2@gmail.com>
 * Copyright (C) 2018, Bin Meng <bmeng.cn@gmail.com>
 * Copyright (C) 2018, Anup Patel <anup@brainfault.org>
 * Copyright (C) 2012 Regents of the University of California
 *
 * RISC-V architecturally-defined generic timer driver
 *
 * This driver provides generic timer support for S-mode U-Boot.
 */

#include <common.h>
#include <dm.h>
#include <errno.h>
#include <timer.h>
#include <asm/csr.h>

#if 1
#undef csr_read
#define csr_read(x) 1
#endif

static u64 notrace riscv_timer_get_count(struct udevice *dev)
{
#if	0
	__maybe_unused u32 hi, lo;

	if (IS_ENABLED(CONFIG_64BIT))
		return csr_read(CSR_TIME);

	do {
		hi = csr_read(CSR_TIMEH);
		lo = csr_read(CSR_TIME);
	} while (hi != csr_read(CSR_TIMEH));

	return ((u64)hi << 32) | lo;
#else
	static u64 counter;
	counter++;
	return counter;
#endif
}

#if CONFIG_IS_ENABLED(LOONGARCH_SMODE) && IS_ENABLED(CONFIG_TIMER_EARLY)
/**
 * timer_early_get_rate() - Get the timer rate before driver model
 */
unsigned long notrace timer_early_get_rate(void)
{
	return LOONGARCH_SMODE_TIMER_FREQ;
}

/**
 * timer_early_get_count() - Get the timer count before driver model
 *
 */
u64 notrace timer_early_get_count(void)
{
	return riscv_timer_get_count(NULL);
}
#endif

static int riscv_timer_probe(struct udevice *dev)
{
	struct timer_dev_priv *uc_priv = dev_get_uclass_priv(dev);

//	printf("TTTTTTTTTTTTTTTTTTTTTTTTTTTTTT\n");
//	while(1);

	/* clock frequency was passed from the cpu driver as driver data */
	uc_priv->clock_rate = dev->driver_data;

	return 0;
}

static const struct timer_ops riscv_timer_ops = {
	.get_count = riscv_timer_get_count,
};

U_BOOT_DRIVER(riscv_timer) = {
	.name = "loongarch_timer",
	.id = UCLASS_TIMER,
	.probe = riscv_timer_probe,
	.ops = &riscv_timer_ops,
	.flags = DM_FLAG_PRE_RELOC,
};
