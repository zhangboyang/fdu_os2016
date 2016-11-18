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
			    __func_, __FILE__, __LINE__, #condition); \
	} while (0)


#ifdef DEBUG
#define kpdebug(...) kprintf("DEBUG: " __VA_ARGS__)
#else
#define kpdebug(...)
#endif




static inline void memdump(void *memaddr, size_t memsize)
{
    int n = memsize;
    unsigned addr = ULCAST(memaddr);
    
    // copied from ZBY's NEMU
    
    printf( "  %d bytes of memory dump at "  "0x%08X"  "\n", n, addr);
    
    printf(
           "            +0 +1 +2 +3 +4 +5 +6 +7  +8 +9 +A +B +C +D +E +F"  "\n");
    //     "  AABBCCDD  aa bb cc dd 00 11 22 33  dd cc bb aa 33 22 11 00  ................");


    /* THE CODE BELOW IS VERY UGLY, BUT IT WORKS!!!
       NOBODY (INCLUDING ZBY) CAN UNDERSTAND IT AFTER FEW DAYS!
       ZBY IS TOO LAZY TO REWRITE IT
    */
    char buf[16];
    char b;
    int m = n % 16 ? n - n % 16 + 16 : n;
    int changed_flag = 0;
    char lb[16];
    int bcflag[16] = {};
    for (i = 0; i < m; i++) {
        if (i % 16 == 0) {
            for (j = 0; j < 16; j++)
                if (i + j < n)
                    lb[j] = *(unsigned char *)(memaddr + i + j);
                else
                    lb[j] = 0xcc;
        
            changed_flag = 0;
            for (j = 0; j < 16; j++)
                if (!new_flag && i + j < n && lb[j] != last_dump[i + j])
                    bcflag[j] = changed_flag = 1;
                else
                    bcflag[j] = 0;
            if (changed_flag)
                printf(  "  %08x  " , addr + i);
            else
                printf( "  %08x  " , addr + i);
        }
        
        if (i < n) {
            b = lb[i % 16];
            if (!bcflag[i % 16])
                printf("%02x ", b & 0xff);
            else
                printf(  "%02x " , b & 0xff);
            last_dump[i] = b;
        } else {
            b = '.';
            printf(".. ");
        }
        buf[i % 16] = isprint(b) ? b : '.';
        
        if (i % 16 == 7)
            printf(" ");
        
        if (i % 16 == 15) {
            printf(" ");
            for (j = 0; j < 16; j++)
                if (!bcflag[j])
                    printf("%c", buf[j]);
                else
                    printf(  "%c" , buf[j]);
            printf("\n");
        }
    }
}

#endif /* !__ASSEMBLER__ */

#endif /* !_AIM_DEBUG_H */

