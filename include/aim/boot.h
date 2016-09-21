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

extern void bootmain(void); 

// bootloader io functions
extern void bioinit(); // bootloader io init (arch-dependent)
extern void bputc(int c); // bootloader putc (arch-dependent)
extern void bputs(const char *str); // bootloader puts
extern int bprintf(const char *fmt, ...); // bootloader printf

#define boot_panic(fmt, ...) \
    do { \
        bputs("=== BOOTLOADER PANIC ==="); \
        bprintf(fmt, ## __VA_ARGS__); \
        while (1); \
    } while (0)

// kernel ELF reader functions
extern void kernreader_init(void);
extern void kernreader_readfile(void *buf, size_t size, size_t offset);

#endif /* !__ASSEMBLER__ */

#include <arch-boot.h>

#endif /* !_AIM_BOOT_H */

