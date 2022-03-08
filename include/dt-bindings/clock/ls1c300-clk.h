/* SPDX-License-Identifier: GPL-2.0 */
/*
 * generated, don't edit.
 * Copyright (C) 2022 Du Huanpeng <dhu@hodcarrier.org>
 */

#ifndef __DT_BINDINGS_LS1C300_CLK_H__
#define __DT_BINDINGS_LS1C300_CLK_H__

/* Base clocks */
#define CLK_XTAL		8
#define CLK_PLL			1
#define CLK_CPU			2
#define CLK_SDRAM               3

#define CLK_CAMERA              4
#define CLK_DC                  5
#define CLK_AXIMUX              6

/* Peripheral clocks */
#define CLK_UART0               (CLK_SDRAM)
#define CLK_UART1               (CLK_SDRAM)
#define CLK_UART2               (CLK_SDRAM)
#define CLK_UART3               (CLK_SDRAM)
#define CLK_UART4               (CLK_SDRAM)
#define CLK_UART5               (CLK_SDRAM)
#define CLK_UART6               (CLK_SDRAM)
#define CLK_UART7               (CLK_SDRAM)
#define CLK_UART8               (CLK_SDRAM)
#define CLK_UART9               (CLK_SDRAM)
#define CLK_UART10              (CLK_SDRAM)
#define CLK_UART11              (CLK_SDRAM)
#define CLK_CAN0                CLK_SDRAM
#define CLK_CAN1                CLK_SDRAM
#define CLK_I2C0                CLK_SDRAM
#define CLK_PWM                 CLK_SDRAM
#define CLK_I2S                 CLK_SDRAM
#define CLK_RTC                 CLK_SDRAM
#define CLK_I2C1                CLK_SDRAM
#define CLK_SDIO                CLK_SDRAM
#define CLK_I2C2                CLK_SDRAM
#define CLK_ADC                 CLK_SDRAM
#define CLK_NAND                CLK_SDRAM

#define CLK_WDT			(CLK_SDRAM)

#endif /* __DT_BINDINGS_LS1C300_CLK_H__ */
