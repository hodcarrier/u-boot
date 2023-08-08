/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Header file for interrupt functions
 *
 * (C) Copyright 2000-2009
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 * Copyright (C) 2023 Du Huanpeng <dhu@hodcarrier.org>
 */

#include <irq_func.h>

int interrupt_init(void){return 0;}
void timer_interrupt(struct pt_regs *regs){}
void external_interrupt(struct pt_regs *regs){}
void irq_install_handler(int vec, interrupt_handler_t *handler, void *arg){}
void irq_free_handler(int vec){}
void reset_timer(void){}

void enable_interrupts(void){}
int disable_interrupts(void){return 0;}
