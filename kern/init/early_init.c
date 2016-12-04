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
#include <aim/console.h>
#include <aim/device.h>
#include <aim/early_kmmap.h>
#include <aim/init.h>
#include <aim/mmu.h>
#include <aim/panic.h>
#include <drivers/io/io-mem.h>
#include <drivers/io/io-port.h>
#include <platform.h>

static inline
int early_devices_init(void)
{
#ifdef IO_MEM_ROOT
	if (io_mem_init(&early_memory_bus) < 0)
		return EOF;
#endif /* IO_MEM_ROOT */

#ifdef IO_PORT_ROOT
	if (io_port_init(&early_port_bus) < 0)
		return EOF;
#endif /* IO_PORT_ROOT */
	return 0;
}

#include <libc/stdio.h>
#include <aim/panic.h>
#include <aim/mmu.h>
#include <aim/initcalls.h>
#include <libc/string.h>

// FIXME: THIS IS THE SUPER DIRTY bprintf() HACK
#if defined(__i386__)
#define bprintf ((int (*)(const char *, ...)) (0x7c00))
#elif defined(__arm__)
#define bprintf (*(int (**)(const char *, ...)) (0x18000))
#endif


__noreturn
void master_early_init(void)
{
    bprintf("this is bprintf, at lowaddr!\n");
    
    #ifdef __arm__
    uint32_t cpsr;
    __asm__ __volatile__ ("mrs %0, cpsr":"=r"(cpsr));
    bprintf("CPSR = %x\n", cpsr);
    /*
        on RPI2 (HYP mode):
        >>> bin(0x600001da)
        '0b1100000000000000000000111011010'
    */
    #endif
    
    char a[] = "abcdefg";
    strlcpy(a, "aaabbb", 3);
    bprintf("%s\n", a);
    char b[] = "abcdefg";
    memmove(b, b + 1, 6);
    bprintf("%s\n", b);
    char c[] = "1234567";
    memmove(c + 1, c, 6);
    bprintf("%s\n", c);
    
    
    /* clear address-space-related callback handlers */
	early_mapping_clear();
	mmu_handlers_clear();

	/* prepare early devices like memory bus and port bus */
	if (early_devices_init() < 0) {
		panic("early_devices_init() failed.");
    }

	if (early_console_init(EARLY_CONSOLE_BUS, EARLY_CONSOLE_BASE, EARLY_CONSOLE_MAPPING) < 0) {
		panic("Early console init failed.\n");
	}
	kprintf("kernel console initialized.\n");
	
	/* other preperations, including early secondary buses */
	arch_early_init();
	
	
	
	do_early_initcalls();

	kputs("Hello, world!\n");
	kprintf("hello world! this is the AIM kernel!\n");

	early_mm_init();
	
	mmu_jump();
}


__noreturn
void slave_early_init(int id)
{
//framebuffer: base=3d800000 width=1440 height=900 pitch=000010e0 format=0
    memset((void *)0x3d800000, -1, 900*0x000010e0);
    arch_slave_early_init(id);
    while (1);
}
