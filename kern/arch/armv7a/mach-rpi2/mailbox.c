#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

// this file is created by ZBY
// FIXME: should this file move to drivers?

// reference:
//  https://github.com/raspberrypi/firmware/wiki/Mailboxes
//  http://elinux.org/RPi_Framebuffer
//  https://github.com/chyyuu/ucore_os_plus/blob/master/ucore/src/kern-ucore/arch/arm/mach-raspberrypi/mailbox.c


#include <sys/types.h>
#include <aim/device.h>
#include <aim/debug.h>
#include <aim/mmu.h>
#include <libc/string.h>
#include <asm.h>
#include <platform.h>
#include <mailbox.h>
#include <drivers/io/io-mem.h>

static struct device __mailbox = {
	.class = DEVCLASS_NON,
};
#define inst (&__mailbox)

int write_mailbox(uint32_t channel, uint32_t data)
{
    //kprintf("write_mailbox(%08x, %08x)\n", channel, data);
    assert((data & 0xF) == 0);
    assert((channel & 0xF) == channel);
    struct bus_device *bus = inst->bus;
	bus_write_fp bus_write32 = bus->bus_driver.get_write_fp(bus, 32);
	if (!bus_write32) return -1;
	bus_read_fp bus_read32 = bus->bus_driver.get_read_fp(bus, 32);
	if (!bus_read32) return -1;
	
	uint64_t s;
	do {
	    dmb(); // FIXME: necessary?
	    int r;
	    if ((r = bus_read32(bus, inst->base, MAIL0_STATUS, &s)) < 0) return r;
	    //kprintf("r=%d s=%08llx s&full=%08llx\n", r, s, (s&MAIL_FULL));
	} while (s & MAIL_FULL);
	
	// bus_write32 should have dmb() in it
	return bus_write32(bus, inst->base, MAIL0_WRITE, (channel | data));
}
int read_mailbox(uint32_t channel, uint32_t *data)
{
    //kprintf("read_mailbox(%08x)\n", channel);
    assert((channel & 0xF) == channel);
    struct bus_device *bus = inst->bus;
	bus_write_fp bus_write32 = bus->bus_driver.get_write_fp(bus, 32);
	if (!bus_write32) return -1;
	bus_read_fp bus_read32 = bus->bus_driver.get_read_fp(bus, 32);
	if (!bus_read32) return -1;
	
    int r;
	uint64_t s;
	do {
	    dmb(); // FIXME: necessary?
	    if ((r = bus_read32(bus, inst->base, MAIL0_STATUS, &s)) < 0) return r;
	} while (s & MAIL_EMPTY);
	
    uint64_t d;
    if ((r = bus_read32(bus, inst->base, MAIL0_READ, &d)) < 0) return r;
    if (data) *data = d;
    //kprintf("  maildata=%08llx\n", d);
    
    return 0;
}




#define PROPERTY_BUFSIZE 4096
#define PROPERTY_DATASIZE (PROPERTY_BUFSIZE - 12)
static __attribute((aligned(MAILBOX_ALIGN))) struct {
    uint32_t size;
    uint32_t code;
    uint8_t data[PROPERTY_DATASIZE];
    uint32_t padding;
} __property_buffer;
#define PROPERTY_CODE_REQUEST 0
#define PROPERTY_CODE_SUCCESS 0x80000000
#define PROPERTY_CODE_ERROR 0x80000001
int ask_property(void *buf, size_t reqsize, size_t bufsize) // buf should point to array of property tags
{
    assert(reqsize <= PROPERTY_DATASIZE);
    assert(bufsize <= PROPERTY_DATASIZE);
    __property_buffer.size = PROPERTY_BUFSIZE;
    __property_buffer.code = PROPERTY_CODE_REQUEST;
    memcpy(__property_buffer.data, buf, reqsize);
    memset(__property_buffer.data + reqsize, 0, 4); // set end tag
    int r;
//dump_memory(&__property_buffer, reqsize + 12);
    if ((r = write_mailbox(MAILBOX_CHANNEL_PROPERTY, PA2VCA(premap_addr(ULCAST(&__property_buffer))))) < 0) return r;
    if ((r = read_mailbox(MAILBOX_CHANNEL_PROPERTY, NULL)) < 0) return r;
//dump_memory(&__property_buffer, reqsize + 12);
    if (__property_buffer.code != PROPERTY_CODE_SUCCESS) return -1;
    memcpy(buf, __property_buffer.data, bufsize);
    return 0;
}


int ask_property_tag(int id, void *buf, size_t reqsize, size_t bufsize, size_t *ressize) // buf should point to raw data
{
    int r;
#define PROPERTY_TAG_VALBUFSIZE (PROPERTY_BUFSIZE - 24)
    assert(reqsize <= PROPERTY_TAG_VALBUFSIZE);
    assert(bufsize <= PROPERTY_TAG_VALBUFSIZE);
    struct {
        struct property_header;
        uint8_t data[PROPERTY_TAG_VALBUFSIZE];
    } tag = {
        .id = id,
        .bufsize = PROPERTY_TAG_VALBUFSIZE,
        .size = reqsize,
        .type = PROPERTY_TAG_REQUEST,
    };
    memcpy(tag.data, buf, reqsize);
    if ((r = ask_property(&tag, sizeof(tag), sizeof(tag))) < 0) return r;
    if (tag.type != PROPERTY_TAG_RESPONSE) return -1;
    memcpy(buf, tag.data, bufsize);
    if (ressize) *ressize = tag.size;
    return 0;
}


// driver framework

static void __mailbox_set_bus(struct bus_device *bus, addr_t base)
{
	__mailbox.bus = bus;
	__mailbox.base = base;
}

static struct bus_device *__mapped_bus;
static addr_t __mapped_base;

static void __jump_handler(void)
{
	__mailbox_set_bus(__mapped_bus, __mapped_base);
}

static void mailbox_initdev(struct bus_device *bus, addr_t base, addr_t mapped_base)
{
    __mailbox_set_bus(bus, base);
	__mapped_bus = (struct bus_device *) postmap_addr(bus);
	__mapped_base = mapped_base;
	if (jump_handlers_add(__jump_handler) != 0) while (1);
}

void mailbox_init()
{
    mailbox_initdev(&early_memory_bus, MAIL_MEMIO_BASE, postmap_addr(MAIL_MEMIO_BASE));
    kprintf("mailbox initialized.\n");
}

