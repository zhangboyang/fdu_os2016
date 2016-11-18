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
    struct {
        int width;
        int height;
    } scrinfo, scrreq;
    
    if (ask_property_tag(MAILBOX_PROP_FB_GETDISPLAYSIZE, &scrinfo, 0, sizeof(scrinfo), NULL) < 0) panic("can't get display size");
    kprintf("screen: width=%d height=%d\n", scrinfo.width, scrinfo.height);
    
    // reference: https://github.com/brianwiddas/pi-baremetal/blob/master/framebuffer.c    
    scrreq = scrinfo;
    if (ask_property_tag(MAILBOX_PROP_FB_SETDISPLAYSIZE, &scrreq, sizeof(scrreq), sizeof(scrreq), NULL) < 0 ||
        memcmp(&scrreq, &scrinfo, sizeof(scrreq)) != 0) panic("can't set display size");
    if (ask_property_tag(MAILBOX_PROP_FB_SETBUFFERSIZE, &scrreq, sizeof(scrreq), sizeof(scrreq), NULL) < 0 ||
        memcmp(&scrreq, &scrinfo, sizeof(scrreq)) != 0) panic("can't set buffer size");
        
    int depthinfo, depthreq;
    depthinfo = depthreq = 24; // FBFMT_R8G8B8
    if (ask_property_tag(MAILBOX_PROP_FB_SETBUFFERSIZE, &depthreq, sizeof(depthreq), sizeof(depthreq), NULL) < 0 ||
        depthinfo != depthreq) panic("can't set depth");
        
    union {
        uint32_t align;
        struct {
            uint32_t base;
            uint32_t size;
        };
    } fbreq = {
        .align = 16,
        .size = 0,
    };
    if (ask_property_tag(MAILBOX_PROP_FB_ALLOCBUFFER, &fbreq, sizeof(fbreq), sizeof(fbreq), NULL) < 0 ||
        fbreq.base == 0) panic("can't alloc framebuffer");
    
    int pitch;
    if (ask_property_tag(MAILBOX_PROP_FB_GETPITCH, &pitch, 0, sizeof(pitch), NULL) < 0 || pitch == 0) panic("can't get pitch");
    
    // set fbinfo
    *fb = (struct fbinfo) {
        .bits = PTRCAST(VCA2PA(fbmail.addr)), // return physical memory address here
        .width = scrinfo.width,
        .height = scrinfo.height,
        .pitch = pitch,
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

