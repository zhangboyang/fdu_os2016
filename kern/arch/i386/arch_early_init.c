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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <libc/stdio.h>
#include <aim/panic.h>

static void check_cpu(void)
{
    bprintf("checking CPUID ... ");
    unsigned eax, ecx, edx, ebx;
    eax = 0x00000001;
    __asm__ __volatile__ ("cpuid":"+eax"(eax),"=ecx"(ecx),"=edx"(edx),"=ebx"(ebx));
    if (!(edx & (1 << 6))) {
        panic("no PAE support in this processor!");
    }
    if (!(edx & (1 << 5))) {
        panic("no RDMSR/WRMSR in this processor!");
    }
    eax = 0x80000001;
    __asm__ __volatile__ ("cpuid":"+eax"(eax),"=ecx"(ecx),"=edx"(edx),"=ebx"(ebx));
    if (!(edx & (1 << 20))) {
        panic("no NX/XD support in this processor!");
    }
    bprintf("OK\n");
}
void arch_early_init(void)
{
    check_cpu();
}


