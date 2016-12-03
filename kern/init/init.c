#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <aim/panic.h>
#include <aim/console.h>
#include <aim/mmu.h>
#include <aim/vmm.h>
#include <aim/trap.h>
#include <aim/initcalls.h>
#include <aim/device.h>
#include <aim/debug.h>
#include <asm.h>
#include <platform.h>

/*
 * this file is added by ZBY
 */

// FIXME: THIS IS THE SUPER DIRTY bprintf() HACK
#if defined(__i386__)
#define bprintf ((int (*)(const char *, ...)) (0x7c00 + KOFFSET))
#else
#define bprintf(...) do {} while (0)
#endif

#ifdef __i386__
int syscall(int num, ...);
__asm__ ( // the syscall()
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
#endif


void master_init()
{
    bprintf("this is bprintf, at highaddr!\n");
    
    fbcls(&fb, 0x000000);
    
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
    
    // init trap
    trap_init();

    // init irq
    irq_init();
    
    // call initcalls
    do_initcalls();
    
    
    /*// test
    syscall(1,2,3,4,5,6,7);
    syscall(2,3,4,5,6,7,1);
    kprintf("press keyboard and you will see IRQ1.\n");
    irq_enable(1); // IRQ1: keyboard
    sti();
    //asm volatile ("movl $0x100, (0xfffffff0)"); // PF
    while (1);*/

#ifdef i386
#define uart_ns16550_devno 162 // FIXME: see uart-ns16550.c
    struct chr_device *chdev = (struct chr_device *) dev_from_id(uart_ns16550_devno);
    chdev->chr_driver.putc(uart_ns16550_devno, 'h');
#endif

    panic("bye");
}

