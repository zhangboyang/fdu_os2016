#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <aim/panic.h>
#include <aim/console.h>
#include <aim/mmu.h>
#include <aim/vmm.h>
#include <aim/trap.h>

/*
 * this file is added by ZBY
 */

// FIXME: dirty hack!
#define bprintf ((int (*)(const char *, ...)) (0x7c00 + KOFFSET))

int __attribute__((__noinline__)) syscall(int id, ...) {
	    int ret;
	    int *args = &id;
	    asm volatile("int $0x80": "=a"(ret) : "a"(args[0]), "b"(args[1]), "c"(args[2]), "d"(args[3]), "esi"(args[4]), "edi"(args[5]), "ebp"(args[6]));
	    return ret;
    }
    
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
    
    
    panic("bye");
}

