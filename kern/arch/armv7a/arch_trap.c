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
#include <aim/vmm.h>


extern uint32_t vectors[];
void trap_init()
{
    // set vectors
    __asm__ __volatile__ ("mcr p15, 0, %0, c12, c0, 0"::"r"(vectors));
    
    // prepare stacks
    struct trap_sp {
    #define TRAP_SP_SIZE 3
        uint32_t abt[TRAP_SP_SIZE];
        uint32_t und[TRAP_SP_SIZE];
        uint32_t irq[TRAP_SP_SIZE];
        uint32_t fiq[TRAP_SP_SIZE];
    };
/*    struct trap_sp *trap_stack = VF(g_pvbridge, malloc, sizeof(struct trap_sp)); // alloc memory for stack
    __asm__ __volatile__(
        "msr sp_abt, %0\n\t"
        "msr sp_und, %1\n\t"
        "msr sp_irq, %2\n\t"
        "msr sp_fiq, %3\n\t"
        ::"r"(trap_stack->abt), "r"(trap_stack->und), "r"(trap_stack->irq), "r"(trap_stack->fiq)
    );*/
    


    
    kprintf("haha\n");
    __asm__ __volatile__ ("mov r0, #0;"
                          "mov r1, #1;"
                          "mov r2, #2;"
                          "mov r3, #3;"
                          "mov r4, #4;"
                          "mov r5, #5;"
                          "mov r6, #6;"
                          "mov r7, #7;"
                          "mov r8, #8;"
                          "mov r9, #9;"
                          "mov r10, #10;"
                          "mov r11, #11;"
                          "mov r12, #12;"
                          "mov r13, #13;"
                          "mov r14, #14;"
//                          "mov r15, #15;"
                          "svc #0");  
    kprintf("bad\n");
}

void trap(struct trapframe *tf)
{
    dump_memory(tf, sizeof(*tf));
    panic("trap!");
}
