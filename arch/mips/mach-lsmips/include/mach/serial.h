/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2020 MediaTek Inc.
 *
 * Author:  Gao Weijie <weijie.gao@mediatek.com>
 *
 * Copyright (C) 2022 Du Huanpeng <dhu@hodcarrier.org>
 */

#ifndef __LSMIPS_SERIAL_H_
#define __LSMIPS_SERIAL_H_

void lsmips_spl_serial_init(void);
int gpio_set_alternate(int gpio, int func);

#endif /* __LSMIPS_SERIAL_H_ */
