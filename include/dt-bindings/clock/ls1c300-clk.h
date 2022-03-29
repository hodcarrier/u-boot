/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2022 Du Huanpeng <dhu@hodcarrier.org>
 */

#ifndef __DT_BINDINGS_LS1C300_CLK_H__
#define __DT_BINDINGS_LS1C300_CLK_H__

/* Base clocks */
#define CLK_XTAL	0
#define CLK_PLL		1
#define CLK_CPU		2
#define CLK_CPU_THROT	7
#define CLK_SDRAM	3

#define CLK_CAMERA	4
#define CLK_DC		5
#define CLK_PIX		5
#define CLK_AXIMUX	6

/* Peripheral clocks */
#define CLK_UART0	3
#define CLK_UART1	3
#define CLK_UART2	3
#define CLK_UART3	3
#define CLK_UART4	3
#define CLK_UART5	3
#define CLK_UART6	3
#define CLK_UART7	3
#define CLK_UART8	3
#define CLK_UART9	3
#define CLK_UART10	3
#define CLK_UART11	3
#define CLK_CAN0	3
#define CLK_CAN1	3
#define CLK_I2C0	3
#define CLK_PWM		3
#define CLK_I2S		3
#define CLK_RTC		3
#define CLK_I2C1	3
#define CLK_SDIO	3
#define CLK_I2C2	3
#define CLK_ADC		3
#define CLK_NAND	3

#define CLK_WDT		3

#endif /* __DT_BINDINGS_LS1C300_CLK_H__ */
