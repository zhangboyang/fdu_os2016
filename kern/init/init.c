#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <aim/panic.h>
#include <aim/console.h>
#include <aim/mmu.h>
#include <aim/vmm.h>
#include <aim/trap.h>
#include <asm.h>

/*
 * this file is added by ZBY
 */

// FIXME: dirty hack!
#define bprintf ((int (*)(const char *, ...)) (0x7c00 + KOFFSET))

int syscall(int num, ...);
__asm__ ( // the abs_jump()
".globl syscall\n"
"syscall:\n"
    "push %ebx\n"
    "push %esi\n"
    "push %edi\n"
    "push %ebp\n"
    
    "mov 20(%esp), %eax\n"
    "mov 24(%esp), %ebx\n"
    "mov 28(%esp), %ecx\n"
    "mov 32(%esp), %edx\n"
    "mov 36(%esp), %esi\n"
    "mov 40(%esp), %edi\n"
    "mov 44(%esp), %ebp\n"
    "int $0x80\n"
    
    "pop %ebp\n"
    "pop %edi\n"
    "pop %esi\n"
    "pop %ebx\n"
    
    "ret\n"
);


void master_init()
{
    bprintf("this is bprintf, at highaddr!\n");
    
    jump_handlers_apply();
    
    kprintf("hello world! we are now at high address!\n");
    
    // bootstrip the pmm and vmm
    vmm_bootstrap();
    /*kprintf("====== kmalloc test ===========\n");
    void *ptr = kmalloc(15, 0);
    kprintf("kmalloc() = %p\n", kmalloc(0x100, 0));
    kprintf("kmalloc() = %p\n", kmalloc(0x100, 0));
    kfree(ptr);
    kprintf("kmalloc() = %p\n", kmalloc(0x100, 0));
    kprintf("kmalloc() = %p\n", kmalloc(0x100, 0));*/

    // init cpu_list[]
    cpu_init_structure();
    cpu_reload(&cpu_list[0]);
    
    // init IRQ
    trap_init();
    
    
    syscall(1,2,3,4,5,6,7);
    syscall(2,3,4,5,6,7,1);
    sti();
    
    panic("bye");
}

