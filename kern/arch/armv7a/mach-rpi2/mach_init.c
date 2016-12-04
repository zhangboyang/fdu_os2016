#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <aim/device.h>
#include <aim/debug.h>
#include <aim/mmu.h>
#include <aim/vmm.h>
#include <aim/init.h>
#include <bcm2836.h>

void mach_master_init(void)
{
    dump_memory((void *)0, 0x180);
    bcm2836_init();
}

uint32_t slave_stack[4];
void mach_start_slaves(void)
{
    kprintf("init slaves ...\n");
    
    extern uint8_t AIM_KERN_STACK_BOTTOM[];
    extern uint8_t AIM_KERN_STACK_START[];
    uint32_t stacksz = AIM_KERN_STACK_BOTTOM - AIM_KERN_STACK_START;
    
    for (int i = 1; i < 3; i++) slave_stack[i] = VF(g_pmalloc, malloc, stacksz);
    
    for (int i = 0; i < 20; i++)    bcm2836_write_mailbox(1, 3, ULCAST(LOWADDR(slave_early_init)));
    
}
