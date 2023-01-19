# SPDX-License-Identifier: GPL-2.0+
#
# Copyright (C) 2017 Andes Technology Corporation.
# Rick Chen, Andes Technology Corporation <rick@andestech.com>

ifeq ($(CONFIG_ARCH_LA64I),y)
	ARCH_BASE = loongarch64
	ABI_BASE = lp64d
endif
ifeq ($(CONFIG_ARCH_LA32I),y)
	ARCH_BASE = loongarch32
	ABI_BASE = ilp32
endif
ifeq ($(CONFIG_LOONGARCH_ISA_A),y)
	ARCH_A = a
endif
ifeq ($(CONFIG_LOONGARCH_ISA_F),y)
	ARCH_F = f
endif
ifeq ($(CONFIG_LOONGARCH_ISA_D),y)
	ARCH_D = d
	ABI_D = d
endif
ifeq ($(CONFIG_LOONGARCH_ISA_C),y)
	ARCH_C = c
endif
ifeq ($(CONFIG_CMODEL_NORMAL),y)
	CMODEL = normal
endif
ifeq ($(CONFIG_CMODEL_LARGE),y)
	CMODEL = large
endif


LOONGARCH_MARCH = $(ARCH_BASE)$(ARCH_A)$(ARCH_F)$(ARCH_D)$(ARCH_C)
ABI = $(ABI_BASE)$(ABI_D)

# Newer binutils versions default to ISA spec version 20191213 which moves some
# instructions from the I extension to the Zicsr and Zifencei extensions.
toolchain-need-zicsr-zifencei := $(call cc-option-yn, -mabi=$(ABI) -march=$(LOONGARCH_MARCH)_zicsr_zifencei)
ifeq ($(toolchain-need-zicsr-zifencei),y)
	LOONGARCH_MARCH := $(LOONGARCH_MARCH)_zicsr_zifencei
endif

ARCH_FLAGS = -march=$(LOONGARCH_MARCH) -mabi=$(ABI) -mcmodel=$(CMODEL)
ARCH_FLAGS = loongarch64 -mabi=lp64d -mcmodel=normal

PLATFORM_CPPFLAGS	+= $(ARCH_FLAGS)
CFLAGS_EFI		+= $(ARCH_FLAGS)

head-y := arch/loongarch/cpu/start.o

libs-y += arch/loongarch/cpu/
libs-y += arch/loongarch/cpu/$(CPU)/
libs-y += arch/loongarch/lib/
