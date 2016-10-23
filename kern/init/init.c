#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <aim/panic.h>
#include <aim/console.h>
#include <aim/mmu.h>

/*
 * this file is added by ZBY
 */

// FIXME: dirty hack!
#define bprintf ((int (*)(const char *, ...)) (0x7c00 + KOFFSET))

void master_init()
{
    bprintf("this is bprintf, at highaddr!\n");
    
    jump_handlers_apply();
    bprintf("apply");
    
    kprintf("hello world! we are now at high address!\n");
    
    panic("bye");
}

