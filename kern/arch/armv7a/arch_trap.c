/*
    this file is added by ZBY
    arch-trap related functions
*/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <aim/console.h>
#include <libc/stdarg.h>
#include <libc/stddef.h>
#include <libc/stdio.h>
#include <aim/debug.h>
#include <aim/mmu.h>
#include <aim/trap.h>
#include <asm.h>


void trap_init()
{
    dump_memory((void *) 0, 0x100);
    dump_memory_aligned((void *) 0, 0x100);
    dump_memory((void *) 5, 0x100);
    dump_memory_aligned((void *) 5, 0x100);
    dump_memory((void *) 15, 0x100);
    dump_memory_aligned((void *) 15, 0x100);
}

void trap(struct trapframe *tf)
{
    panic("trap!");
}
