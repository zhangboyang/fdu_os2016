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

