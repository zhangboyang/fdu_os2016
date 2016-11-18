#ifndef _MACH_PLATFORM_H
#define _MACH_PLATFORM_H


#define RPI2_MEMIO_BASE  0x3F000000

// videocore physical memory offset
#define RPI2_VC_POFFSET 0x40000000

// physical address to video core address
#define PA2VCA(addr) (addr + RPI2_VC_POFFSET)
#define VCA2PA(addr) (addr - RPI2_VC_POFFSET)



/* for UART */
#define UART_BASE RPI2_MEMIO_BASE 

#define EARLY_CONSOLE_BUS	(&early_memory_bus)
#define EARLY_CONSOLE_BASE	UART_BASE
#define EARLY_CONSOLE_MAPPING	MAP_LINEAR

/* mailbox api */
#define MAILBOX_ALIGN 16
#define MAILBOX_CHANNEL_PROPERTY 8
int write_mailbox(uint32_t channel, uint32_t data);
int read_mailbox(uint32_t channel, uint32_t *data);
void mailbox_init();
int ask_property(void *buf, size_t reqsize, size_t bufsize);

#define MAILBOX_PROP_ARMMEMORY 0x00010005
#define MAILBOX_PROP_VCMEMORY 0x00010006
#define MAILBOX_PROP_FB_GETDISPLAYSIZE 0x00040003
#define MAILBOX_PROP_FB_ALLOCBUFFER 0x00040001
#define MAILBOX_PROP_FB_SETDISPLAYSIZE 0x00048003
#define MAILBOX_PROP_FB_SETBUFFERSIZE 0x00048004
#define MAILBOX_PROP_FB_SETDEPTH 0x00048005
#define MAILBOX_PROP_FB_GETPITCH 0x00040008
int ask_property_tag(int id, void *buf, size_t reqsize, size_t bufsize, size_t *ressize); // ressize = response size


/* framebuffer */
enum {
    FBFMT_R8G8B8,
};
struct fbinfo {
    void *bits;
    unsigned int width;
    unsigned int height;
    unsigned int pitch;
    int format;
};
int fbinit(struct fbinfo *fb);

#endif /* _MACH_PLATFORM_H */

