#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <aim/debug.h>
#include <aim/mmu.h>
#include <libc/string.h>
#include <platform.h>


int fbinit(struct fbinfo *fb)
{
    int r;
    
    struct scrinfo_t {
        int width;
        int height;
    } scrinfo;
    
    
    if ((r = ask_property_tag(MAILBOX_PROP_FB_GETDISPLAYSIZE, &scrinfo, 0, sizeof(scrinfo), NULL)) < 0) return r;
    kprintf("screen: width=%d height=%d\n", scrinfo.width, scrinfo.height);

    int depth = 24;

    // make request
    // reference: https://github.com/brianwiddas/pi-baremetal/blob/master/framebuffereq.c
    // reference: https://github.com/chyyuu/ucore_os_plus/blob/master/ucore/src/kern-ucore/arch/arm/mach-raspberrypi/framebuffer.c
    
    struct {
        struct property_header dispreq;
        struct scrinfo_t disp;
        
        struct property_header bufreq;
        struct scrinfo_t buf;

        struct property_header depthreq;
        int depth;
        
        struct property_header fbreq;
        union {
            int align;
            struct {
                uint32_t base;
                uint32_t size;
            };
        } fb;
    } req;
    
    req.dispreq = (struct property_header) {
        .id = MAILBOX_PROP_FB_SETDISPLAYSIZE,
        .bufsize = sizeof(req.disp),
        .size = sizeof(req.disp),
        .type = PROPERTY_TAG_REQUEST,
    };
    req.disp = scrinfo;

    req.bufreq = (struct property_header) {
        .id = MAILBOX_PROP_FB_SETBUFFERSIZE,
        .bufsize = sizeof(req.buf),
        .size = sizeof(req.buf),
        .type = PROPERTY_TAG_REQUEST,
    };
    req.buf = scrinfo;

    req.depthreq = (struct property_header) {
        .id = MAILBOX_PROP_FB_SETDEPTH,
        .bufsize = sizeof(uint32_t),
        .size = sizeof(uint32_t),
        .type = PROPERTY_TAG_REQUEST,
    };
    req.depth = depth;
  
    req.fbreq = (struct property_header) {
        .id = MAILBOX_PROP_FB_ALLOCBUFFER,
        .bufsize = sizeof(req.fb),
        .size = sizeof(req.fb.align),
        .type = PROPERTY_TAG_REQUEST,
    };
    req.fb.align = 1024 * 1024 * 2;
    
//dump_memory(&req, sizeof(req));
    if ((r = ask_property(&req, sizeof(req), sizeof(req))) < 0) return r;
//dump_memory(&req, sizeof(req));
    
    // check values
    if (memcmp(&req.disp, &scrinfo, sizeof(struct scrinfo_t)) != 0) return -1;
    if (memcmp(&req.buf, &scrinfo, sizeof(struct scrinfo_t)) != 0) return -1;
    if (req.depth != depth) return -1;
    if (!req.fb.base || !req.fb.size) return -1;
    
    // get pitch
    int pitch;
    if ((r = ask_property_tag(MAILBOX_PROP_FB_GETPITCH, &pitch, 0, sizeof(pitch), NULL)) < 0) return r;
    
    // set fbinfo
    *fb = (struct fbinfo) {
        .bits = PTRCAST(req.fb.base - RPI2_VC_POFFSET_NOL2CACHE), // return physical memory address here
        .width = scrinfo.width,
        .height = scrinfo.height,
        .pitch = pitch,
        .format = FBFMT_R8G8B8,
    };
    
    kprintf("framebuffer: base=%08p width=%d height=%d pitch=%08x format=%d\n", fb->bits, fb->width, fb->height, fb->pitch, fb->format);
    
    return 0;
    
    // FIXME: i don't know why this can't work
/*
#define MAILBOX_CHANNEL_FRAMEBUFFER 1
    // construct mail
    // reference: https://github.com/raspberrypi/firmware/wiki/Mailbox-framebuffer-interface
    static __attribute((aligned(MAILBOX_ALIGN))) struct fbmailinfo {
        uint32_t width;
        uint32_t height;
        uint32_t virt_width;
        uint32_t virt_height;
        uint32_t pitch;
        uint32_t depth;
        uint32_t xoffset;
        uint32_t yoffset;
        uint32_t addr;
        uint32_t size;
    } fbmail;
    fbmail = (struct fbmailinfo) {
        .width = scrinfo.width,
        .height = scrinfo.height,
        .virt_width = scrinfo.width,
        .virt_height = scrinfo.height,
        .pitch = 0,
        .depth = 24, // FBFMT_R8G8B8
        .xoffset = 0,
        .yoffset = 0,
        .addr = 0,
        .size = 0,
    };
    
dump_memory(&fbmail, sizeof(fbmail));
    int r;
    if ((r = write_mailbox(MAILBOX_CHANNEL_FRAMEBUFFER, PA2VCA(premap_addr(ULCAST(&fbmail))))) < 0) return r;
    uint32_t d;
    if ((r = read_mailbox(MAILBOX_CHANNEL_FRAMEBUFFER, &d)) < 0) return r;
kprintf("d=%d\n", d); dump_memory(&fbmail, sizeof(fbmail));
    if (d != 1) return -1;

    // set fbinfo
    *fb = (struct fbinfo) {
        .bits = PTRCAST(VCA2PA(fbmail.addr)), // return physical memory address here
        .width = fbmail.width,
        .height = fbmail.height,
        .pitch = fbmail.pitch,
        .format = FBFMT_R8G8B8,
    };*/
}

void fbcls(struct fbinfo *fbdev, uint32_t color) // color should be X8R8G8B8
{
    assert(fbdev->format == FBFMT_R8G8B8);
    uint8_t r = color >> 16;
    uint8_t g = color >> 8;
    uint8_t b = color;
    struct {
        uint8_t b;
        uint8_t g;
        uint8_t r;
    } *prow;
    int drow, dcol;
    for (drow = 0; drow < fbdev->height; drow++) {
        prow = fbdev->bits + fbdev->pitch * drow;
        for (dcol = 0; dcol < fbdev->width; dcol++) {
            prow[dcol].r = r;
            prow[dcol].g = g;
            prow[dcol].b = b;
        }
    }
}

static void fbdrawpt(struct fbinfo *fbdev, int x, int y, uint32_t color)
{
    assert(fbdev->format == FBFMT_R8G8B8);
    struct {
        uint8_t b;
        uint8_t g;
        uint8_t r;
    } *pixel = fbdev->bits + fbdev->pitch * x + y * 3;
    pixel->r = color >> 16;
    pixel->g = color >> 8;
    pixel->b = color;
}

__asm__ ("fbfont:\n.incbin \"terminal.font\"\n");
extern uint8_t fbfont[256][8][6];
void fbdrawch(struct fbinfo *fbdev, int x, int y, uint32_t fgcolor, uint32_t bgcolor, int ch)
{
    int i, j;
    for (i = 0; i < 9; i++) {
        for (j = 0; j < 6; j++) {
            fbdrawpt(fbdev, x + i, y + j, (i < 8 ? fbfont[ch][i][j] : 0) ? fgcolor : bgcolor);
        }
    }
}

