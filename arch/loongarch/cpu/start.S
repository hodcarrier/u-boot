/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Startup Code for RISC-V Core
 *
 * Copyright (c) 2017 Microsemi Corporation.
 * Copyright (c) 2017 Padmarao Begari <Padmarao.Begari@microsemi.com>
 *
 * Copyright (C) 2017 Andes Technology Corporation
 * Rick Chen, Andes Technology Corporation <rick@andestech.com>
 */

#include <asm-offsets.h>
#include <config.h>
#include <common.h>
#include <elf.h>
#include <system-constants.h>
#include <asm/encoding.h>
#include <generated/asm-offsets.h>
#include <asm/regdef.h>

#ifdef CONFIG_32BIT
#define LREG			ld.w
#define SREG			st.w
#define REGBYTES		4
#define RELOC_TYPE		R_LARCH_32
#define SYM_INDEX		0x8
#define SYM_SIZE		0x10
#else
#define LREG			ld.d
#define SREG			st.d
#define REGBYTES		8
#define RELOC_TYPE		R_LARCH_64
#define SYM_INDEX		0x20
#define SYM_SIZE		0x18
#endif



.section .data
secondary_harts_relocation_error:
	.ascii "Relocation of secondary harts has failed, error %d\n"

.section .text
.globl _start
_start:
#ifndef CONFIG_SPL_BUILD
	la	a1, __dtb_dt_begin		// HACK: embedded fdt address
#endif
#if CONFIG_IS_ENABLED(LOONGARCH_MMODE)
	csrrd	a0, 0x20			// CPUID
#endif



	/*
	 * Save hart id and dtb pointer. The thread pointer register is not
	 * modified by C code. It is used by secondary_hart_loop.
	 */
	move	tp, a0
	move	s1, a1

	/*
	 * Set the global data pointer to a known value in case we get a very
	 * early trap. The global data pointer will be set its actual value only
	 * after it has been initialized.
	 */
	move	u0, zero

	/*
	 * Set the trap handler. This must happen after initializing u0 because
	 * the handler may use it.
	 */
	la	t0, trap_entry
	csrwr	t0, 0xC				// EENTRY

	/*
	 * Mask all interrupts. Interrupts are disabled globally (in m/sstatus)
	 * for U-Boot, but we will need to read m/sip to determine if we get an
	 * IPI
	 */
	ori	t0, zero, 1<<2			// IE bit ??
	csrxchg	zero, t0, 0			// CRMD   ??

#if CONFIG_IS_ENABLED(SMP)
	/* check if hart is within range */
	/* tp: hart id */
	ori	t0, zero, CONFIG_NR_CPUS
	bge	tp, t0, hart_out_of_bounds_loop

	/* set xSIE bit to receive IPIs */
#if CONFIG_IS_ENABLED(LOONGARCH_MMODE)
	ori	t0, zero, MIE_MSIE
#else
	ori	t0, zero, SIE_SSIE
#endif
	move	t0, zero			// TODO: what is xSIE ?
	csrxchg	zero, t0, 0			// CRMD, no effort
#endif

/*
 * Set stackpointer in internal/ex RAM to call board_init_f
 */
call_board_init_f:
#if defined(CONFIG_SPL_BUILD) && defined(CONFIG_SPL_STACK)
	li	sp, CONFIG_SPL_STACK
#else
	li.d	sp, SYS_INIT_SP_ADDR
#endif
	ori	t0, zero, 15
	andn	sp, sp, t0		/* force 16 byte alignment */


call_board_init_f_0:
	move	a0, sp
	bl	board_init_f_alloc_reserve

	/*
	 * Save global data pointer for later. We don't set it here because it
	 * is not initialized yet.
	 */
	move	s0, a0

	/* setup stack */
#if CONFIG_IS_ENABLED(SMP)
	/* tp: hart id */
	slli.d	t0, tp, CONFIG_STACK_SIZE_SHIFT
	sub.d	sp, a0, t0
#else
	move	sp, a0
#endif

	/* Configure proprietary settings and customized CSRs of harts */
call_harts_early_init:
	bl	harts_early_init

#if !CONFIG_IS_ENABLED(XIP)
	/*
	 * Pick hart to initialize global data and run U-Boot. The other harts
	 * wait for initialization to complete.
	 */
	la	t0, hart_lottery	// arch/loongarch/cpu/cpu.c
	ori	t1, zero, 1
//	amoswap.w s2, t1, 0(t0)
	amswap.w s2, t1, t0
	bnez	s2, wait_for_gd_init
#else
	/*
	 * FIXME: u0 is set before it is initialized. If an XIP U-Boot ever
	 * encounters a pending IPI on boot it is liable to jump to whatever
	 * memory happens to be in ipi_data.addr on boot. It may also run into
	 * problems if it encounters an exception too early (because printf/puts
	 * accesses gd).
	 */
	move	u0, s0
#if CONFIG_IS_ENABLED(LOONGARCH_MMODE)
	bnez	tp, secondary_hart_loop
#endif
#endif

	move	a0, s0
	bl	board_init_f_init_reserve

	SREG	s1, u0,  GD_FIRMWARE_FDT_ADDR
	/* save the boot hart id to global_data */
	SREG	tp, u0,  GD_BOOT_HART

#if !CONFIG_IS_ENABLED(XIP)
#ifdef CONFIG_AVAILABLE_HARTS
	la	t0, available_harts_lock
	amoswap.w.rl zero, zero, 0(t0)
#endif

wait_for_gd_init:
	/*
	 * Set the global data pointer only when gd_t has been initialized.
	 * This was already set by arch_setup_gd on the boot hart, but all other
	 * harts' global data pointers gets set here.
	 */
	move	u0, s0
#ifdef CONFIG_AVAILABLE_HARTS
	la	t0, available_harts_lock
	ori	t1, zero, 1
1:	amoswap.w.aq t1, t1, 0(t0)
	bnez	t1, 1b

	/* register available harts in the available_harts mask */
	ori	t1, zero, 1
	sll	t1, t1, tp
	LREG	t2, u0, GD_AVAILABLE_HARTS
	or	t2, t2, t1
	SREG	t2, u0, GD_AVAILABLE_HARTS

	amoswap.w.rl zero, zero, 0(t0)
#endif

	/*
	 * Continue on hart lottery winner, others branch to
	 * secondary_hart_loop.
	 */
	bnez	s2, secondary_hart_loop
#endif

	/* Enable cache */
	bl	icache_enable
	bl	dcache_enable

#ifdef CONFIG_DEBUG_UART
	bl	debug_uart_init
#endif

	move	a0, zero		/* a0 <-- boot_flags = 0 */
	bl	 board_init_f		/* jump to board_init_f() */

#ifdef CONFIG_SPL_BUILD
spl_clear_bss:
	la	t0, __bss_start
	la	t1, __bss_end
	beq	t0, t1, spl_stack_gd_setup

spl_clear_bss_loop:
	SREG	zero, t0,  0
	addi.d	t0, t0, REGBYTES
	blt	t0, t1, spl_clear_bss_loop

spl_stack_gd_setup:
	bl	spl_relocate_stack_gd

	/* skip setup if we did not relocate */
	beqz	a0, spl_call_board_init_r
	move	s0, a0

	/* setup stack on main hart */
#if CONFIG_IS_ENABLED(SMP)
	/* tp: hart id */
	slli.d	t0, tp, CONFIG_STACK_SIZE_SHIFT
	sub.d	sp, s0, t0
#else
	move	sp, s0
#endif

#if CONFIG_IS_ENABLED(SMP)
	/* set new stack and global data pointer on secondary harts */
spl_secondary_hart_stack_gd_setup:
	la	a0, secondary_hart_relocate
	move	a1, s0
	move	a2, s0
	move	a3, zero
	bl	smp_call_function

	/* hang if relocation of secondary harts has failed */
	beqz	a0, 1f
	move	a1, a0
	la	a0, secondary_harts_relocation_error
	bl	printf
	bl	hang
#endif

	/* set new global data pointer on main hart */
1:	move	u0, s0

spl_call_board_init_r:
	move	a0, zero
	move	a1, zero
	bl	board_init_r
#endif

/*
 * void relocate_code(addr_sp, gd, addr_moni)
 *
 * This "function" does not return, instead it continues in RAM
 * after relocating the monitor code.
 *
 */
.globl relocate_code
relocate_code:
	move	s2, a0			/* save addr_sp */
	move	s3, a1			/* save addr of gd */
	move	s4, a2			/* save addr of destination */

/*
 *Set up the stack
 */
stack_setup:
#if CONFIG_IS_ENABLED(SMP)
	/* tp: hart id */
	slli.d	t0, tp, CONFIG_STACK_SIZE_SHIFT
	sub.d	sp, s2, t0
#else
	move	sp, s2
#endif

	la	t0, _start
	sub.d	t6, s4, t0		/* t6 <- relocation offset */
	beq	t0, s4, clear_bss	/* skip relocation */

	move	t1, s4			/* t1 <- scratch for copy_loop */
	la	t3, __bss_start
	sub.d	t3, t3, t0		/* t3 <- __bss_start_ofs */
	add.d	t2, t0, t3		/* t2 <- source end address */

copy_loop:
	LREG	t5, t0, 0
	addi.d	t0, t0, REGBYTES
	SREG	t5, t1,  0
	addi.d	t1, t1, REGBYTES
	blt	t0, t2, copy_loop

/*
 * Update dynamic relocations after board_init_f
 */
fix_rela_dyn:
	la	t1, __rel_dyn_start
	la	t2, __rel_dyn_end
	beq	t1, t2, clear_bss
	add.d	t1, t1, t6		/* t1 <- rela_dyn_start in RAM */
	add.d	t2, t2, t6		/* t2 <- rela_dyn_end in RAM */

/*
 * skip first reserved entry: address, type, addend
 */
	b	10f

6:
	LREG	t5, t1, -(REGBYTES*2)	/* t5 <-- relocation info:type */
	ori	t3, zero, R_LARCH_RELATIVE	/* reloc type R_LARCH_RELATIVE */
	bne	t5, t3, 8f		/* skip non-LOONGARCH_RELOC entries */
	LREG	t3, t1, -(REGBYTES*3)
	LREG	t5, t1, -(REGBYTES)	/* t5 <-- addend */
	add.d	t5, t5, t6		/* t5 <-- location to fix up in RAM */
	add.d	t3, t3, t6		/* t3 <-- location to fix up in RAM */
	SREG	t5, t3,  0
	b	10f

8:
	la	t4, __dyn_sym_start
	add.d	t4, t4, t6

9:
	LREG	t5, t1, -(REGBYTES*2) 	/* t5 <-- relocation info:type */
	srli.d	t0, t5, SYM_INDEX	/* t0 <--- sym table index */
	andi	t5, t5, 0xFF		/* t5 <--- relocation type */
	ori	t3, zero, RELOC_TYPE
	bne	t5, t3, 10f		/* skip non-addned entries */

	LREG	t3, t1, -(REGBYTES*3)
	ori	t5, zero, SYM_SIZE
	mul.d	t0, t0, t5
	add.d	s5, t4, t0
	LREG	t0, t1, -(REGBYTES)	/* t0 <-- addend */
	LREG	t5, s5, REGBYTES
	add.d	t5, t5, t0
	add.d	t5, t5, t6		/* t5 <-- location to fix up in RAM */
	add.d	t3, t3, t6		/* t3 <-- location to fix up in RAM */
	SREG	t5, t3,  0
10:
	addi.d	t1, t1, (REGBYTES*3)
	ble	t1, t2, 6b

/*
 * trap update
*/
	la	t0, trap_entry
	add.d	t0, t0, t6
	csrwr	t0, 0xC				// EENTRY

clear_bss:
	la	t0, __bss_start		/* t0 <- rel __bss_start in FLASH */
	add.d	t0, t0, t6		/* t0 <- rel __bss_start in RAM */
	la	t1, __bss_end		/* t1 <- rel __bss_end in FLASH */
	add.d	t1, t1, t6		/* t1 <- rel __bss_end in RAM */
	beq	t0, t1, relocate_secondary_harts

1:
	SREG	zero, t0, 0		/* clear loop... */
	addi.d	t0, t0, REGBYTES
	blt	t0, t1, 1b

relocate_secondary_harts:
#if CONFIG_IS_ENABLED(SMP)
	/* send relocation IPI */
	la	t0, secondary_hart_relocate
	add.d	a0, t0, t6

	/* store relocation offset */
	move	s5, t6

	move	a1, s2
	move	a2, s3
	move	a3, zero
	bl	smp_call_function

	/* hang if relocation of secondary harts has failed */
	beqz	a0, 1f
	move	a1, a0
	la	a0, secondary_harts_relocation_error
	bl	printf
	bl	hang

	/* restore relocation offset */
1:	move	t6, s5
#endif

/*
 * We are done. Do not return, instead branch to second part of board
 * initialization, now running from RAM.
 */
call_board_init_r:
	bl	invalidate_icache_all
	bl	flush_dcache_all
	la	ra, board_init_r        /* offset of board_init_r() */
	add.d	ra, ra, t6		/* real address of board_init_r() */

/*
 * setup parameters for board_init_r
 */
	move	a0, s3			/* gd_t */
	move	a1, s4			/* dest_addr */

/*
 * jump to it ...
 */
	jr	ra			/* jump to board_init_r() */

#if CONFIG_IS_ENABLED(SMP)
hart_out_of_bounds_loop:
	/* Harts in this loop are out of bounds, increase CONFIG_NR_CPUS. */
	idle	0
	b	hart_out_of_bounds_loop

/* SMP relocation entry */
secondary_hart_relocate:
	/* a1: new sp */
	/* a2: new gd */
	/* tp: hart id */

	/* setup stack */
	slli.d	t0, tp, CONFIG_STACK_SIZE_SHIFT
	sub.d	sp, a1, t0

	/* update global data pointer */
	move	u0, a2
#endif

/*
 * Interrupts are disabled globally, but they can still be read from m/sip. The
 * wfi function will wake us up if we get an IPI, even if we do not trap.
 */
secondary_hart_loop:
	idle	0

#if CONFIG_IS_ENABLED(SMP)
//	csrrd	t0, MODE_PREFIX(ip)
	csrrd	t0, 0
#if CONFIG_IS_ENABLED(LOONGARCH_MMODE)
	andi	t0, t0, MIE_MSIE
#else
	andi	t0, t0, SIE_SSIE
#endif
	beqz	t0, secondary_hart_loop

	move	a0, tp
	bl	handle_ipi
#endif

	b	secondary_hart_loop
