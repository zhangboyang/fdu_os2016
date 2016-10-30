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

struct gatedesc idt[256];
extern uint32_t vectors[];

void init_idt()
{
    for (int i = 0; i < 256; i++) {
        SETGATE(idt[i], 0, SEG_KCODE<<3, vectors[i], 0);
    }
    SETGATE(idt[T_SYSCALL], 1, SEG_KCODE<<3, vectors[T_SYSCALL], DPL_USER);
}

void trap_init()
{
    init_idt();
    lidt(idt);
}

void trap(struct trapframe *tf)
{
    if (tf->trapno == T_SYSCALL) {
        // number and return value: eax
        //  arg1   arg2   arg3   arg4   arg5   arg6
        //  ebx    ecx    edx    esi    edi    ebp
        tf->eax = handle_syscall(tf->eax, tf->ebx, tf->ecx, tf->edx, tf->esi, tf->edi, tf->ebp);
    } else {
        handle_interrupt(tf->trapno);
    }
    trap_return(tf);
}
