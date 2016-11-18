#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <aim/debug.h>
#include <platform.h>

void mach_early_init()
{
    mailbox_init();
    
    struct meminfo {
        uint32_t base;
        uint32_t size;
    } arminfo, vcinfo;
    if (ask_property_tag(MAILBOX_PROP_ARMMEMORY, &arminfo, 0, sizeof(arminfo)) < 0) panic("can't get memory info for ARM");
    if (ask_property_tag(MAILBOX_PROP_VCMEMORY, &vcinfo, 0, sizeof(vcinfo)) < 0) panic("can't get memory info for VideoCore");
    
    kprintf("arm memory: base=0x%x size=0x%x\n", r, arminfo.base, arminfo.size, NULL);
    kprintf("vc memory: base=0x%x size=0x%x\n", r, vcinfo.base, vcinfo.size, NULL);
    
}
