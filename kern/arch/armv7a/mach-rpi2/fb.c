#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <aim/debug.h>
#include <aim/mmu.h>
#include <platform.h>


int fbinit(struct fbinfo *fb)
{
    struct {
        int width;
        int height;
    } screeninfo;
    if (ask_property_tag(MAILBOX_PROP_GETDISPLAYSIZE, &screeninfo, 0, sizeof(screeninfo), NULL) < 0) panic("can't get display size");
    kprintf("screen: width=%d height=%d\n", screeninfo.width, screeninfo.height);
    
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
        .width = screeninfo.width,
        .height = screeninfo.height,
        .virt_width = screeninfo.width,
        .virt_height = screeninfo.height,
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
    };
    
    return 0;
}

