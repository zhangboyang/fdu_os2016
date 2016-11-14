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


#include <arch-mmu.h>

                
#ifndef __ASSEMBLER__

#include <elf.h>
typedef struct elf32hdr		elf_hdr;
typedef struct elf32_phdr	elf_phdr;
extern uint8_t mbr[];



static inline unsigned char inb(unsigned short port)
{
    unsigned char data;
    __asm__ __volatile__ ("inb %1, %0": "=a"(data): "d"(port));
    return data;
}
static inline void outb(unsigned short port, unsigned char data)
{
    __asm__ __volatile__ ("outb %0, %1":: "a"(data), "d"(port));
}
static inline void insl(int port, void *addr, int cnt)
{
    __asm __volatile__ ("cld; rep insl" :
                        "=D" (addr), "=c" (cnt) :
                        "d" (port), "0" (addr), "1" (cnt) :
                        "memory", "cc");
}

static inline void bmemset(void *s, int c, size_t n)
{
    __asm__ __volatile__ ("cld; rep stosb":"+a"(c), "+D"(s), "+c"(n):: "memory", "cc");
}

static inline void bmemcpy(void *dest, const void *src, size_t n)
{
    __asm__ __volatile__ ("cld; rep movsb":"+S"(src), "+D"(dest), "+c"(n):: "memory", "cc");
}


extern int readsect_realmode(unsigned lowaddr, unsigned int sect);

#endif /* !__ASSEMBLER__ */

#endif /* !_ARCH_BOOT_H */

