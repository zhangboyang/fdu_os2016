#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <aim/debug.h>
#include <libc/string.h>
#include <platform.h>


void mach_early_init()
{
//    dump_memory(NULL, 0x8000);
    
    mailbox_init();
    
    struct meminfo {
        uint32_t base;
        uint32_t size;
    } arminfo, vcinfo;
    if (ask_property_tag(MAILBOX_PROP_ARMMEMORY, &arminfo, 0, sizeof(arminfo), NULL) < 0) panic("can't get memory info for ARM");
    if (ask_property_tag(MAILBOX_PROP_VCMEMORY, &vcinfo, 0, sizeof(vcinfo), NULL) < 0) panic("can't get memory info for VideoCore");
    kprintf("arm memory: base=0x%08x size=0x%08x\n", arminfo.base, arminfo.size);
    kprintf("vc memory: base=0x%08x size=0x%08x\n", vcinfo.base, vcinfo.size);
    
    static struct fbinfo fbdev;
    if (fbinit(&fbdev) < 0) panic("can't init framebuffer");
    
    dump_memory(&fbdev, sizeof(fbdev));
    memset(fbdev.bits, -1, fbdev.width * fbdev.pitch);
}
