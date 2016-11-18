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
#include <asm.h>
#include <platform.h>
#include <mailbox.h>

static struct device __mailbox = {
	.class = DEVCLASS_NON,
};
#define inst (&__mailbox)

int write_mailbox(uint32_t channel, uint32_t data)
{
    assert((channel & 0xF) == 0);
    struct bus_device *bus = inst->bus;
	bus_write_fp bus_write32 = bus->bus_driver.get_write_fp(bus, 32);
	if (!bus_write32) return -1;
	bus_read_fp bus_read32 = bus->bus_driver.get_read_fp(bus, 32);
	if (!bus_read32) return -1;
	
	uint64_t s;
	do {
	    dmb(); // FIXME: necessary?
	    int r;
	    if ((r = bus_read32(bus, inst->base, MAIL1_STATUS, &s)) < 0) return r;
	} while (s | MAIL_FULL);
	
	// bus_write32 should have dmb() in it
	return bus_write32(bus, inst->base, MAIL0_WRITE, (channel | data));
}
int read_mailbox()
{
    
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
}

void mailbox_init()
{
    mailbox_initdev(&early_memory_bus, MAIL_MEMIO_BASE, postmap_addr(MAIL_MEMIO_BASE));
}

