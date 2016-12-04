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
{
    struct bus_device *bus = inst->bus;
	bus_read_fp bus_read32 = bus->bus_driver.get_read_fp(bus, 32);
	if (!bus_read32) return -1;
	
    uint64_t d;
    int r;
    if ((r = bus_read32(bus, inst->base, MAIL0_READ, &d)) < 0) return r;
    *data = d;
    return 0;
}

int bcm2836_writereg(uint32_t reg, uint32_t data)
{
    struct bus_device *bus = inst->bus;
	bus_write_fp bus_write32 = bus->bus_driver.get_write_fp(bus, 32);
	if (!bus_write32) return -1;
	
    return bus_write32(bus, inst->base, reg, data);
}

int bcm2836_write_mailbox(int core, int id, uint32_t value)
{
    // FIXME: this function is not atomic!
    kprintf("write bcm2836 mailbox, core %d mailbox %d, value %08x.\n", core, id, value);
    uint32_t r_rdclr = CORE0_MBOX0_RDCLR + 0x10 * core + 0x4 * id; // read, write-clear register
    uint32_t r_set = CORE0_MBOX0_SET + 0x10 * core + 0x4 * id; // write-set register
    int r;
uint32_t d;
if ((r = bcm2836_readreg(r_rdclr, &d)) < 0) return r;
kprintf("  read = %08x\n", d);

    if ((r = bcm2836_writereg(r_rdclr, -1)) < 0) return r; // clear all bits
if ((r = bcm2836_readreg(r_rdclr, &d)) < 0) return r;
kprintf("  read = %08x\n", d);
    if ((r = bcm2836_writereg(r_set, value)) < 0) return r; // write to set value
    
    return 0;
}


void bcm2836_init()
{
    inst->bus = &early_memory_bus;
    inst->base = postmap_addr(BCM2836_MEMIO_BASE);
    kprintf("bcm2836 initialized.\n");
}
