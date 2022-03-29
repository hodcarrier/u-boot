// SPDX-License-Identifier: GPL-2.0
/*
 * Watchdog driver for MediaTek SoCs
 *
 * Copyright (C) 2018 MediaTek Inc.
 * Author: Ryder Lee <ryder.lee@mediatek.com>
 *
 * based on: drivers/watchdog/mtk_wdt.c
 * Copyright (C) 2020-2022 Du Huanpeng <dhu@hodcarrier.org>
 */

#include <common.h>
#include <dm.h>
#include <hang.h>
#include <wdt.h>
#include <asm/io.h>
#include <linux/bitops.h>
#include <clk.h>

#define WDT_EN		(priv->base + 0)
#define WDT_TIMER	(priv->base + 4)
#define WDT_SET		(priv->base + 8)

struct lsmips_wdt_priv {
	void __iomem *base;
	ulong clock;
	unsigned long timeout;
};

static int lsmips_wdt_reset(struct udevice *dev)
{
	struct lsmips_wdt_priv *priv = dev_get_priv(dev);

	writel(priv->timeout, WDT_TIMER);
	writel(1, WDT_SET);

	return 0;
}

static int lsmips_wdt_stop(struct udevice *dev)
{
	struct lsmips_wdt_priv *priv = dev_get_priv(dev);

	writel(0, WDT_EN);
	return 0;
}

static int lsmips_wdt_expire_now(struct udevice *dev, ulong flags)
{
	struct lsmips_wdt_priv *priv = dev_get_priv(dev);

	writel(1, WDT_EN);
	writel(1, WDT_TIMER);
	writel(1, WDT_SET);

	hang();
	return 0;
}

static int lsmips_wdt_start(struct udevice *dev, u64 timeout_ms, ulong flags)
{
	struct lsmips_wdt_priv *priv = dev_get_priv(dev);
	unsigned int timeout;

	timeout = U32_MAX / (priv->clock / 1000);

	if (timeout < timeout_ms)
		timeout = U32_MAX;
	else
		timeout = timeout_ms * (priv->clock / 1000);

	debug("WDT: reload  = %08x\n", timeout);

	writel(1, WDT_EN);
	writel(timeout, WDT_TIMER);
	writel(1, WDT_SET);

	priv->timeout = timeout;

	return 0;
}

static int lsmips_wdt_probe(struct udevice *dev)
{
	struct lsmips_wdt_priv *priv = dev_get_priv(dev);
	struct clk cl;

	priv->base = dev_remap_addr(dev);
	if (!priv->base)
		return -ENOENT;

	if (clk_get_by_index(dev, 0, &cl) == 0)
		priv->clock = clk_get_rate(&cl);

	if (priv->clock < 33000000 || priv->clock > 150000000) {
		/* assume 67MHz by default */
		priv->clock = 67108864;
	}

	debug("WDT: clock = %ld\n", priv->clock);

	writel(0, WDT_EN);
	return 0;
}

static const struct wdt_ops lsmips_wdt_ops = {
	.start = lsmips_wdt_start,
	.reset = lsmips_wdt_reset,
	.stop = lsmips_wdt_stop,
	.expire_now = lsmips_wdt_expire_now,
};

static const struct udevice_id lsmips_wdt_ids[] = {
	{ .compatible = "loongson,ls1c300-wdt"},
	{}
};

U_BOOT_DRIVER(lsmips_wdt) = {
	.name = "lsmips_wdt",
	.id = UCLASS_WDT,
	.of_match = lsmips_wdt_ids,
	.priv_auto = sizeof(struct lsmips_wdt_priv),
	.probe = lsmips_wdt_probe,
	.ops = &lsmips_wdt_ops,
	.flags = DM_FLAG_PRE_RELOC,
};
