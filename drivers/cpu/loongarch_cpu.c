// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2018, Bin Meng <bmeng.cn@gmail.com>
 * Copyright (C) 2020, Sean Anderson <seanga2@gmail.com>
 */

#include <clk.h>
#include <common.h>
#include <cpu.h>
#include <dm.h>
#include <errno.h>
#include <log.h>
#include <asm/global_data.h>
#include <dm/device-internal.h>
#include <dm/lists.h>
#include <linux/bitops.h>
#include <linux/err.h>

DECLARE_GLOBAL_DATA_PTR;

static int riscv_cpu_get_desc(const struct udevice *dev, char *buf, int size)
{
	const char *isa;

	isa = dev_read_string(dev, "riscv,isa");
	if (size < (strlen(isa) + 1))
		return -ENOSPC;

	strcpy(buf, isa);

	return 0;
}

static int riscv_cpu_get_info(const struct udevice *dev, struct cpu_info *info)
{
	int ret;
	struct clk clk;
	const char *mmu;
	u32 i_cache_size;
	u32 d_cache_size;

	/* First try getting the frequency from the assigned clock */
	ret = clk_get_by_index((struct udevice *)dev, 0, &clk);
	if (!ret) {
		ret = clk_get_rate(&clk);
		if (!IS_ERR_VALUE(ret))
			info->cpu_freq = ret;
		clk_free(&clk);
	}

	if (!info->cpu_freq)
		dev_read_u32(dev, "clock-frequency", (u32 *)&info->cpu_freq);

	mmu = dev_read_string(dev, "mmu-type");
	if (mmu)
		info->features |= BIT(CPU_FEAT_MMU);

	/* check if I cache is present */
	ret = dev_read_u32(dev, "i-cache-size", &i_cache_size);
	if (ret)
		/* if not found check if d-cache is present */
		ret = dev_read_u32(dev, "d-cache-size", &d_cache_size);

	/* if either I or D cache is present set L1 cache feature */
	if (!ret)
		info->features |= BIT(CPU_FEAT_L1_CACHE);

	return 0;
}

static int riscv_cpu_get_count(const struct udevice *dev)
{
	ofnode node;
	int num = 0;

	ofnode_for_each_subnode(node, dev_ofnode(dev->parent)) {
		const char *device_type;

		/* skip if hart is marked as not available in the device tree */
		if (!ofnode_is_enabled(node))
			continue;

		device_type = ofnode_read_string(node, "device_type");
		if (!device_type)
			continue;
		if (strcmp(device_type, "cpu") == 0)
			num++;
	}

	return num;
}

static int riscv_cpu_bind(struct udevice *dev)
{
	struct cpu_plat *plat = dev_get_parent_plat(dev);
	struct driver *drv;
	int ret;

	/* save the hart id */
	// plat->cpu_id = dev_read_addr(dev);
	plat->cpu_id = gd->arch.boot_hart;
	/* first examine the property in current cpu node */
	// ret = dev_read_u32(dev, "timebase-frequency", &plat->timebase_freq);
	/* if not found, then look at the parent /cpus node */
	//if (ret)
	//	dev_read_u32(dev->parent, "timebase-frequency",
	//		     &plat->timebase_freq);
	plat->timebase_freq = 1000000;

	/*
	 * Bind riscv-timer driver on boot hart.
	 *
	 * We only instantiate one timer device which is enough for U-Boot.
	 * Pass the "timebase-frequency" value as the driver data for the
	 * timer device.
	 *
	 * Return value is not checked since it's possible that the timer
	 * driver is not included.
	 */
	if (plat->cpu_id == gd->arch.boot_hart && plat->timebase_freq) {
		drv = lists_driver_lookup_name("loongarch_timer");
		if (!drv) {
			debug("Cannot find the timer driver, not included?\n");
			return 0;
		}

		device_bind_with_driver_data(dev, drv, "loongarch_timer",
					     plat->timebase_freq, ofnode_null(),
					     NULL);
//		while(1);
	}

	return 0;
}

static int riscv_cpu_probe(struct udevice *dev)
{
	int ret = 0;
	struct clk clk;

	/* Get a clock if it exists */
	ret = clk_get_by_index(dev, 0, &clk);
	if (ret)
		return 0;

	ret = clk_enable(&clk);
	clk_free(&clk);
	if (ret == -ENOSYS || ret == -ENOTSUPP)
		return 0;
	else
		return ret;
}

static const struct cpu_ops riscv_cpu_ops = {
	.get_desc	= riscv_cpu_get_desc,
	.get_info	= riscv_cpu_get_info,
	.get_count	= riscv_cpu_get_count,
};

static const struct udevice_id riscv_cpu_ids[] = {
	{ .compatible = "loongarch" },
	{ }
};

U_BOOT_DRIVER(riscv_cpu) = {
	.name = "riscv_cpu",
	.id = UCLASS_CPU,
	.of_match = riscv_cpu_ids,
	.bind = riscv_cpu_bind,
	.probe = riscv_cpu_probe,
	.ops = &riscv_cpu_ops,
	.flags = DM_FLAG_PRE_RELOC,
};
