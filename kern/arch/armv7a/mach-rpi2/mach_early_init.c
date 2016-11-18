#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <aim/debug.h>
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
    
    struct {
        int width;
        int height;
    } screeninfo;
    if (ask_property_tag(MAILBOX_PROP_GETDISPLAYSIZE, &screeninfo, 0, sizeof(screeninfo), NULL) < 0) panic("can't get display size");
    kprintf("screen: width=%d height=%d\n", screeninfo.width, screeninfo.height);
    
}
