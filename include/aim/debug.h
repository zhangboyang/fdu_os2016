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

#ifndef _AIM_DEBUG_H
#define _AIM_DEBUG_H

#ifndef __ASSEMBLER__

#include <aim/console.h>

/* 
 * We just want several function here, avoid include hell please.
 */
__noreturn
void panic(const char *fmt, ...);

#define assert(condition) \
	do { \
		if (!(condition)) \
			panic("Assertion failed in %s (%s:%d): %s\n", \
			    __func__, __FILE__, __LINE__, #condition); \
	} while (0)


#ifdef DEBUG
#define kpdebug(...) kprintf("DEBUG: " __VA_ARGS__)
#else
#define kpdebug(...)
#endif



#include <util.h>
#define dump_memory(addr, size) do { \
    kprintf("memory dump requested at %s (%s:%d)\n", __func__, __FILE__, __LINE__); \
    __dump_memory(addr, size); \
} while (0)
static inline void __dump_memory(void *memaddr, size_t memsize)
{
    int n = memsize;
    unsigned addr = ULCAST(memaddr);
    
    
    // copied from ZBY's NEMU
    
    kprintf( " %d bytes of memory dump at "  "0x%08x"  "\n", n, addr);
    
    kprintf("            +0 +1 +2 +3 +4 +5 +6 +7  +8 +9 +A +B +C +D +E +F"  "\n");
    //      "  AABBCCDD  aa bb cc dd 00 11 22 33  dd cc bb aa 33 22 11 00  ................"

    char buf[16];
    char b;
    int m = n % 16 ? n - n % 16 + 16 : n;
    char lb[16];
    int i, j;
    for (i = ROUNDDOWN(ULCAST(memaddr), 16) - memaddr; i < m; i++) {
        if (i % 16 == 0) {
            for (j = 0; j < 16; j++)
                if (i + j < n)
                    lb[j] = *(unsigned char *)(memaddr + i + j);
            kprintf("  %08x  " , addr + i);
        }
        if (i >= 0 && i < n) {
            b = lb[i % 16];
            kprintf("%02x ", b & 0xff);
        } else {
            b = '.';
            kprintf(".. ");
        }
        buf[i % 16] = (b >= ' ' && b <= '~') ? b : '.';
        if (i % 16 == 7)
            kprintf(" ");
        if (i % 16 == 15) {
            kprintf(" ");
            for (j = 0; j < 16; j++)
                kprintf("%c", buf[j]);
            kprintf("\n");
        }
    }
    kprintf(" end of memory dump\n");
}

#endif /* !__ASSEMBLER__ */

#endif /* !_AIM_DEBUG_H */

