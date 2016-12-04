#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <aim/device.h>
#include <aim/debug.h>
#include <aim/mmu.h>
#include <aim/vmm.h>
#include <aim/pmm.h>
#include <aim/init.h>
#include <bcm2836.h>

void mach_master_init(void)
{
    dump_memory((void *)0, 0x180);
    bcm2836_init();
}

uint32_t slave_stack[RPI2_CORES];
uint32_t slave_stack_lowaddr[RPI2_CORES];
void mach_start_slaves(void)
{
    kprintf("init slaves ...\n");
    
    extern uint8_t AIM_KERN_STACK_BOTTOM[];
    extern uint8_t AIM_KERN_STACK_START[];
    uint32_t stacksz = AIM_KERN_STACK_BOTTOM - AIM_KERN_STACK_START;
    
    for (int i = 1; i < RPI2_CORES; i++) {
        slave_stack[i] = VF(pmm_zone[ZONE_NORMAL].allocator, malloc, stacksz);
        slave_stack_lowaddr[i] = premap_addr(slave_stack[i]);
        printf(" stack for slave %d: low %08x high %08x\n", i, slave_stack_lowaddr[i], slave_stack[i]);
    }
    
//    for (int i = 1; i < RPI2_CORES; i++) {
    int i = 1; {
        bcm2836_write_mailbox(i, 3, ULCAST(LOWADDR(slave_early_entry)));
    }
}
