/* Copyright (C) 2016 David Gao <davidgao1001@gmail.com>
 *
 * This file is part of AIM.
 *
 * AIM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * AIM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _ARCH_BOOT_H
#define _ARCH_BOOT_H

/*
    memory map:
    BOOTLOADER_START              ELF_INFO_START     ELF_START
    [***somecode***     **stack**][*****elf-info****][***elffile***]
*/

#define BOOTLOADER_START 0x8000
#define ELF_INFO_START 0x18000
#define ELF_START 0x20000

#ifndef __ASSEMBLER__

typedef struct elf32hdr		elf_hdr;
typedef struct elf32_phdr	elf_phdr;

void *memset(void *s, int c, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
#define bmemset memset
#define bmemcpy memcpy

#endif /* !__ASSEMBLER__ */

#endif /* !_ARCH_BOOT_H */

