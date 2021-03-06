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

#ifndef _AIM_BOOT_H
#define _AIM_BOOT_H

#ifndef __ASSEMBLER__

extern uint8_t text_begin[];
extern uint8_t text_end[];

// there is no "stdarg.h" in bootloader
// we should add it by ourselves
// taken from GCC's stdarg.h
typedef __builtin_va_list __gnuc_va_list;
typedef __gnuc_va_list va_list;
#define va_start(v,l)   __builtin_va_start(v,l)
#define va_end(v)   __builtin_va_end(v)
#define va_arg(v,l) __builtin_va_arg(v,l)


extern void bootmain(void); 

// bootloader io functions
extern void bioinit(); // bootloader io init (arch-dependent)
extern void bputc(int c); // bootloader putc (arch-dependent)
extern void bputs(const char *str); // bootloader puts
extern int bprintf(const char *fmt, ...); // bootloader printf

#define bpanic(fmt, ...) \
    do { \
        bputc('\n'); \
        bputs("=== BOOTLOADER PANIC ==="); \
        bprintf(fmt, ## __VA_ARGS__); \
        bputc('\n'); \
        while (1); \
    } while (0)

// crc32
extern uint32_t crc32(uint32_t crc, const void *buf, size_t size);

// kernel ELF reader functions
extern void kernreader_init(void);
extern void kernreader_readfile(void *buf, size_t size, size_t offset);

// detect memory functions
extern void detectmemory();

#endif /* !__ASSEMBLER__ */

#include <arch-boot.h>

#endif /* !_AIM_BOOT_H */

