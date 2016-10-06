# Copyright (C) 2016 David Gao <davidgao1001@gmail.com>
#
# This file is part of AIM.
#
# AIM is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# AIM is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

include $(top_srcdir)/env.am

SUBDIRS = arch

BUILT_SOURCES = boot.lds

noinst_PROGRAMS = boot.elf
nodist_noinst_DATA = \
	boot.bin boot.s
CLEANFILES = \
	boot.bin boot.s

BOOT_LIBS = \
	arch/$(ARCH)/libentry.la \
	arch/$(ARCH)/libarch.la

boot_elf_SOURCES = \
	bootmain.c
boot_elf_LDADD = $(BOOT_LIBS)
EXTRA_boot_elf_DEPENDENCIES = boot.lds
boot_elf_CPPFLAGS = $(AM_CPPFLAGS_NOPIC)
boot_elf_CFLAGS = $(AM_CFLAGS_NOPIC) -Os
boot_elf_CCASFLAGS = $(AM_CCASFLAGS_NOPIC)
boot_elf_LDFLAGS = $(AM_LDFLAGS_NOPIC) -T boot.lds

# This file has double suffix, makefile rule cannot wildcard him
boot.lds: $(srcdir)/arch/$(ARCH)/boot.lds.S
	$(AM_V_GEN) $(COMPILE) -E -P $^ -o $@

