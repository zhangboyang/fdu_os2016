#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <aim/debug.h>
#include <platform.h>

void mach_early_init()
{
    mailbox_init();
    
    struct {
        uint32_t membase;
        uint32_t memsize;
    } meminfo;
    int r;
    r = ask_property_tag(MAILBOX_PROP_ARMMEMORY, &meminfo, 0, sizeof(meminfo));
    kprintf("r=%d base=0x%x len=0x%x\n", r, meminfo.membase, meminfo.memsize);
}
