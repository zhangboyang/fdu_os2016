#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <aim/device.h>
#include <aim/debug.h>
#include <bcm2836.h>

void mach_master_init(void)
{
    dump_memory((void *)0, 0x180);
    bcm2836_init();
}

void mach_start_slaves(void)
{
    kprintf("init slaves ...\n");
    bcm2836_write_mailbox(1, 3, ULCAST(0x8000));
}
