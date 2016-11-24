#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <aim/debug.h>
#include <aim/mmu.h>
#include <libc/string.h>
#include <platform.h>
#include <aim/early_kmmap.h>

__asm__ ("splash_image_data:\n.incbin \"splash.rgb\"\n");

//__asm__ ("jtxj:\n.incbin \"jtxj.rgb\"\n");

static void copyrow(struct fbinfo *fbdev, int dstrow, int dstcol, void *srcdata, int srcrow, int srcpitch, int srcdepth, int pixelcount)
{
    assert(fbdev->format == FBFMT_R8G8B8);
    assert(srcdepth == 24);
    int dstdepth = 24;
    
    unsigned char *dst = fbdev->bits + dstrow * fbdev->pitch + dstcol * (24 / 8);
    unsigned char *src = srcdata + srcrow * srcpitch;
    
    while (pixelcount--) {
        dst[0] = src[2];
        dst[1] = src[1];
        dst[2] = src[0];
        src += srcdepth / 8;
        dst += dstdepth / 8;
    }
}

void show_splash(struct fbinfo *fbdev, void *imgdata, int imgwidth, int imgheight, int imgdepth)
{
    int drow, dcol;
    for (drow = 0; drow < fbdev->height; drow++) {
        int row = drow % imgheight;
        for (dcol = 0; dcol < fbdev->width; dcol += imgwidth) {
            int count = min(imgwidth, fbdev->width - dcol);
            copyrow(fbdev, drow, dcol, imgdata, row, imgwidth * (imgdepth / 8), imgdepth, count);
        }
    }
}




struct fbinfo fb;

static struct meminfo {
    uint32_t base;
    uint32_t size;
} arminfo, vcinfo;
    
void mach_early_init()
{
//    dump_memory(NULL, 0x8000);
    
    mailbox_init();
    

    if (ask_property_tag(MAILBOX_PROP_ARMMEMORY, &arminfo, 0, sizeof(arminfo), NULL) < 0) panic("can't get memory info for ARM");
    if (ask_property_tag(MAILBOX_PROP_VCMEMORY, &vcinfo, 0, sizeof(vcinfo), NULL) < 0) panic("can't get memory info for VideoCore");
    kprintf("arm memory: base=0x%08x size=0x%08x\n", arminfo.base, arminfo.size);
    kprintf("vc memory: base=0x%08x size=0x%08x\n", vcinfo.base, vcinfo.size);
    
    if (fbinit(LOWADDR(&fb)) < 0) panic("can't init framebuffer");
    
    
//    dump_memory(&fbdev, sizeof(fbdev));
/*    dump_memory(fbdev.bits, 0xA0);
    memset(fbdev.bits, -1, fbdev.height * fbdev.pitch);
    dump_memory(fbdev.bits, 0xA0);*/
    

    extern uint8_t splash_image_data[]; show_splash(LOWADDR(&fb), LOWADDR(splash_image_data), 175, 100, 24);

    
//    extern uint8_t jtxj[]; show_splash(&fbdev, LOWADDR(jtxj), 318, 346, 24);
}


#define RPI2_MAX_MEMORY 0x40000000
void mach_add_mapping()
{
    init_jmphigh_mapping(RPI2_MAX_MEMORY, arminfo.size);

    struct early_mapping entry;
    
    // add device memory
    entry = (struct early_mapping) {
		.paddr	= RPI2_MEMIO_BASE,
		.vaddr	= PTRCAST(RPI2_MEMIO_BASE + KOFFSET),
		.size	= RPI2_MEMIO_TOP - RPI2_MEMIO_BASE,
		.type	= EARLY_MAPPING_DEVICE,
	};
	if (early_mapping_add(&entry) < 0) panic("can't add device memory");
	
    // add framebuffer memory
    struct fbinfo *fbdev = LOWADDR(&fb);
    size_t fbsize = fbdev->height * fbdev->pitch;
    entry = (struct early_mapping) {
		.paddr	= ADDRCAST(fbdev->bits),
		.vaddr	= fbdev->bits + KOFFSET,
		.size	= ROUNDUP(fbsize, BIGPAGE_SIZE),
		.type	= EARLY_MAPPING_FRAMEBUFFER,
	};
	if (early_mapping_add(&entry) < 0) panic("can't add framebuffer memory");
}

