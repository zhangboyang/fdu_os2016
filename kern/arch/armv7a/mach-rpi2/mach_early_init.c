#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <aim/debug.h>
#include <aim/mmu.h>
#include <aim/pmm.h>
#include <libc/string.h>
#include <platform.h>
#include <aim/early_kmmap.h>

////// framebuffer

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
#define CONPAGE_COLS 80
#define CONPAGE_COL_SPACE 0
static int pagecols;
static int conrcols, conrrows; // real cols and rows
static void fbcondrawch(int x, int y, int ch)
{
    int pagecolid = x / conrrows;
    y += pagecolid * (CONPAGE_COLS + CONPAGE_COL_SPACE);
    x %= conrrows;
    int colorid = pagecolid % 2;
    if (ch == '\0') { colorid = 2; ch = ' '; }
    uint32_t bgcolor[] = {0, 0x404040, 0xffff00}, fgcolor[] = {-1, -1, -1};
    fbdrawch(&fb, x * 8, y * 6, fgcolor[colorid], bgcolor[colorid], ch);
}

static int concols, conrows; // logical cols and rows
static int conx = 0, cony = 0;
static int fbconenable = 0;
void fbputc(int ch)
{
    if (!fbconenable) return;
    //fbcondrawch(conx, cony, ' ');
    int xflag = 0;
    if (ch == '\n') { conx++; xflag = 1; }
    else if (ch == '\r') cony = 0;
    else {
        fbcondrawch(conx, cony, ch);
        cony++;
        if (cony >= concols) {
            cony = 0;
            conx++;
            xflag = 1;
        }
    }
    if (conx >= conrows) {
        conx = 0;
    }
    if (xflag) {
        int i, r;
        for (r = 0; r <= 5; r++) {
            if ((conx + r) % conrows == 0) break;
            for (i = 0; i < concols; i++) {
                fbcondrawch(conx + r, i, ' ');
            }
        }
    }
    fbcondrawch(conx, cony, '\0');
}
void fbconcls()
{
    int i, j;
    for (i = 0; i < conrows; i++) {
        for (j = 0; j < concols; j++) {
            fbputc(' ');
        }
    }
    conx = 0;
    cony = 0;
}

static struct meminfo {
    uint32_t base;
    uint32_t size;
} arminfo, vcinfo;
static void rpi2_fbdev_jumphandler(void)
{
    fb.bits += KOFFSET;
    fbconenable = 1;
    fbcls(&fb, 0x000000);
    fbconcls();
}
static void rpi2_fbdev_init()
{
    struct fbinfo *fbdev = LOWADDR(&fb);
    // init fb
    if (fbinit(fbdev) < 0) panic("can't init framebuffer");
    jump_handlers_add(rpi2_fbdev_jumphandler);
    fbcls(fbdev, 0x00ff00);
//    extern uint8_t splash_image_data[]; show_splash(LOWADDR(&fb), LOWADDR(splash_image_data), 175, 100, 24);
//    extern uint8_t jtxj[]; show_splash(&fb, jtxj, 318, 346, 24);
    // init fb console
    conrrows = fbdev->height / 8;
    conrcols = fbdev->width / 6;
    
    pagecols = (conrcols - CONPAGE_COL_SPACE) / CONPAGE_COLS;
    concols = CONPAGE_COLS;
    conrows = conrrows * pagecols;
}









    
static void rpi2_detect_memory()
{
    if (ask_property_tag(MAILBOX_PROP_ARMMEMORY, &arminfo, 0, sizeof(arminfo), NULL) < 0) panic("can't get memory info for ARM");
    if (ask_property_tag(MAILBOX_PROP_VCMEMORY, &vcinfo, 0, sizeof(vcinfo), NULL) < 0) panic("can't get memory info for VideoCore");
    kprintf("arm memory: base=0x%08x size=0x%08x\n", arminfo.base, arminfo.size);
    kprintf("vc memory: base=0x%08x size=0x%08x\n", vcinfo.base, vcinfo.size);
    assert(arminfo.base == 0);
}


void mach_early_init()
{
    mailbox_init();
    rpi2_detect_memory();
    rpi2_fbdev_init();
}

size_t size_after_kernel;

#define RPI2_MAX_MEMORY 0x40000000
void mach_add_mapping()
{
    init_jmphigh_mapping(RPI2_MAX_MEMORY, arminfo.size);
    *LOWADDR(&size_after_kernel) = arminfo.size - ULCAST(KERN_END_LOW);
    
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

void mach_init_pmm_zone()
{
    kprintf("init pmm zone\n");
    pmm_zone[ZONE_NORMAL] = (struct zone) {
        .base = arminfo.base, .size = arminfo.size,
        .page_size = PAGE_SIZE,
    };
}
void mach_init_free_pmm_zone(addr_t kstart, addr_t kend)
{
    kprintf("free non-kernel pages, kstart = %016llx, kend = %016llx\n", kstart, kend);
    struct zone *z = &pmm_zone[ZONE_NORMAL];
    lsize_t pa;
    for (pa = z->base; pa < z->base + z->size; pa += z->page_size) {
        if (pa >= kstart && pa < kend) {
            VF(z->allocator, free, pa);
//            kprintf("free pa = %016llx\n", pa);
        }
    }
}
