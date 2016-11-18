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
    struct scrinfo_t {
        int width;
        int height;
    } scrinfo;
    
    
    if (ask_property_tag(MAILBOX_PROP_FB_GETDISPLAYSIZE, &scrinfo, 0, sizeof(scrinfo), NULL) < 0) panic("can't get display size");
    kprintf("screen: width=%d height=%d\n", scrinfo.width, scrinfo.height);


    
    // reference: https://github.com/brianwiddas/pi-baremetal/blob/master/framebuffereq.c    
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
memset(&req, 0, sizeof(req));
dump_memory(&req, sizeof(req));
    
    req.dispreq = (struct property_header) {
        .id = MAILBOX_PROP_FB_SETDISPLAYSIZE,
        .bufsize = sizeof(req.disp),
        .size = sizeof(req.disp),
        .type = PROPERTY_TAG_REQUEST,
    };
    req.disp = scrinfo;
dump_memory(&req, sizeof(req));    
    req.bufreq = (struct property_header) {
        .id = MAILBOX_PROP_FB_SETBUFFERSIZE,
        .bufsize = sizeof(req.buf),
        .size = sizeof(req.buf),
        .type = PROPERTY_TAG_REQUEST,
    };
    req.buf = scrinfo;
dump_memory(&req, sizeof(req));    
    req.depthreq = (struct property_header) {
        .id = MAILBOX_PROP_FB_SETDEPTH,
        .bufsize = sizeof(uint32_t),
        .size = sizeof(uint32_t),
        .type = PROPERTY_TAG_REQUEST,
    };
    req.depth = 24;
dump_memory(&req, sizeof(req));    
    req.fbreq = (struct property_header) {
        .id = MAILBOX_PROP_FB_SETDISPLAYSIZE,
        .bufsize = sizeof(req.fb),
        .size = sizeof(req.fb.align),
        .type = PROPERTY_TAG_REQUEST,
    };
    req.fb.align = 16;
    
dump_memory(&req, sizeof(req));
    int r;
    if ((r = ask_property(&req, sizeof(req), sizeof(req))) < 0) return r;
dump_memory(&req, sizeof(req));
    
    
    // set fbinfo
    *fb = (struct fbinfo) {
        .bits = PTRCAST(VCA2PA(req.fb.base)), // return physical memory address here
        .width = scrinfo.width,
        .height = scrinfo.height,
        .pitch = 0, // FIXME
        .format = FBFMT_R8G8B8,
    };
    
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

