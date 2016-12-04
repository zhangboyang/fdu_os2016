#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

// this file is created by ZBY
// FIXME: should this file move to drivers?


// local peripherals for bcm2836
// see QA7_rev3.4.pdf for details

#include <sys/types.h>
#include <aim/device.h>
#include <aim/debug.h>
#include <aim/mmu.h>
#include <libc/string.h>
#include <asm.h>
#include <platform.h>
#include <bcm2836.h>
#include <drivers/io/io-mem.h>


static struct device __bcm2836 = {
	.class = DEVCLASS_NON,
};
#define inst (&__bcm2836)


int bcm2836_readreg(uint32_t reg, uint32_t *data)
{assert(0);
}

int bcm2836_writereg(uint32_t reg, uint32_t data)
{
    struct bus_device *bus = inst->bus;
	bus_write_fp bus_write32 = bus->bus_driver.get_write_fp(bus, 32);
	if (!bus_write32) return -1;
	
    return bus_write32(bus, inst->base, reg, data);
}

void bcm2836_init()
{
    inst->bus = &early_memory_bus;
    inst->base = postmap_addr(BCM2836_MEMIO_BASE);
    kprintf("bcm2836 initialized.\n");
}
