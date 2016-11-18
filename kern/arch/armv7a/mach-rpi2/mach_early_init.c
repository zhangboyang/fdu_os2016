#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <aim/debug.h>
#include <libc/string.h>
#include <platform.h>


__asm__ (
    "__imgdata:\n"
    ".incbin \"splash.rgb\"\n"
);
static void copyrow(struct fbinfo *fbdev, int dstrow, int dstcol, void *src, int srcrow, int srcpitch, int srcdepth, int pixelcount)
{
    assert(fbdev->format == FBFMT_R8G8B8);
    assert(srcdepth == 24);
    void *dst = fbdev->bits + dstrow * fbdev->pitch + dstcol * 24;
    src = src + srcrow * srcpitch;
    memcpy(dst, src, pixelcount * (srcdepth / 8));
}
static void show_splash(struct fbinfo *fbdev)
{
    int imgwidth = 318, imgheight = 346;
    int imgdepth = 24;
    extern uint8_t __imgdata[];
    void *imgdata = premap_addr(__imgdata);
    int drow, dcol;
    for (drow = 0; drow < fbdev->height; drow++) {
        int row = drow % imgheight;
        for (dcol = 0; dcol < fbdev->width; dcol += imgwidth) {
            int count = min(imgwidth, fbdev->width - dcol);
            copyrow(fbdev, drow, dcol, imgdata, row, imgwidth * (imgdepth / 8), imgdepth, count);
        }
    }
}

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
    dump_memory(fbdev.bits, 0xA0);
    memset(fbdev.bits, -1, fbdev.height * fbdev.pitch);
    dump_memory(fbdev.bits, 0xA0);
    
    show_splash(&fbdev);
}


