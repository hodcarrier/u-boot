# SPDX-License-Identifier: GPL-2.0+
#
# (C) Copyright 2000-2002
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# Copyright (c) 2017 Microsemi Corporation.
# Padmarao Begari, Microsemi Corporation <padmarao.begari@microsemi.com>
#
# Copyright (C) 2017 Andes Technology Corporation
# Rick Chen, Andes Technology Corporation <rick@andestech.com>
#

32bit-emul		:= elf32loongarch
64bit-emul		:= elf64loongarch

ifdef CONFIG_32BIT
KBUILD_LDFLAGS		+= -m $(32bit-emul)
EFI_LDS			:= elf_loongarch32_efi.lds
endif

ifdef CONFIG_64BIT
KBUILD_LDFLAGS		+= -m $(64bit-emul)
EFI_LDS			:= elf_loongarch64_efi.lds
endif

CONFIG_STANDALONE_LOAD_ADDR ?= 0x00000000

PLATFORM_CPPFLAGS	+= -ffixed-$r21 -fpic
PLATFORM_RELFLAGS	+= -fno-common -gdwarf-2 -ffunction-sections -fdata-sections
LDFLAGS_u-boot		+= --gc-sections -static -pie

EFI_CRT0		:= crt0_loongarch_efi.o
EFI_RELOC		:= reloc_loongarch_efi.o
