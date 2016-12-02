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


extern uint32_t vectors[];
void trap_init()
{
    __asm__ __volatile__ ("mcr p15, 0, %0, c12, c0, 0"::"r"(vectors));
    __asm__ __volatile__ ("svc #0");
}

void trap(struct trapframe *tf)
{
    panic("trap!");
}
