/*
 * this file added by ZBY
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <aim/console.h>
#include <aim/device.h>
#include <aim/mmu.h>
#include <arch-mmu.h>
#include <aim/initcalls.h>
#include <aim/debug.h>
#include <uart-rpi2-hw.h>
#include <asm.h>


// driver instance code

static void __uart_rpi2_init(struct chr_device *inst)
{
    // get bus fp
    struct bus_device *bus = inst->bus;
	bus_write_fp bus_write32 = bus->bus_driver.get_write_fp(bus, 32);
	if (!bus_write32) while (1);
	bus_read_fp bus_read32 = bus->bus_driver.get_read_fp(bus, 32);
	if (!bus_read32) while (1);
	
	// do init staff
	uint64_t ra;
    bus_write32(bus, inst->base, AUX_ENABLES, 1);
    bus_write32(bus, inst->base, AUX_MU_IER_REG, 0);
    bus_write32(bus, inst->base, AUX_MU_CNTL_REG, 0);
    bus_write32(bus, inst->base, AUX_MU_LCR_REG, 3);
    bus_write32(bus, inst->base, AUX_MU_MCR_REG, 0);
    bus_write32(bus, inst->base, AUX_MU_IER_REG, 0);
    bus_write32(bus, inst->base, AUX_MU_IIR_REG, 0xC6);
    bus_write32(bus, inst->base, AUX_MU_BAUD_REG, 270);
    bus_read32(bus, inst->base, GPFSEL1, &ra);
    ra &= ~(7 << 12); //gpio14
    ra |= 2 << 12;    //alt5
    ra &= ~(7 << 15); //gpio15
    ra |= 2 << 15;    //alt5
    bus_write32(bus, inst->base, GPFSEL1, ra);
    bus_write32(bus, inst->base, GPPUD, 0);
    for(ra = 0; ra < 150; ra++) nop(); // FIXME: what's this
    bus_write32(bus, inst->base, GPPUDCLK0, (1 << 14) | (1 << 15));
    for(ra = 0; ra < 150; ra++) nop();
    bus_write32(bus, inst->base, GPPUDCLK0, 0);
    bus_write32(bus, inst->base, AUX_MU_CNTL_REG, 3);
}

static int __uart_rpi2_putchar(struct chr_device *inst, unsigned char c)
{
    // get bus fp
    struct bus_device *bus = inst->bus;
	bus_write_fp bus_write32 = bus->bus_driver.get_write_fp(bus, 32);
	if (!bus_write32) while (1);
	bus_read_fp bus_read32 = bus->bus_driver.get_read_fp(bus, 32);
	if (!bus_read32) while (1);
	
	// do output
	uint64_t data;
	while (!((bus_read32(bus, inst->base, AUX_MU_LSR_REG, &data), data) & 0x20));
    bus_write32(bus, inst->base, AUX_MU_IO_REG, c);
	
    return 0;
}


// driver framework code

static struct bus_device *__mapped_bus;
static addr_t __mapped_base;
static struct chr_device __early_uart_rpi2 = {
	.class = DEVCLASS_CHR,
};

static void __early_console_set_bus(struct bus_device *bus, addr_t base)
{
	__early_uart_rpi2.bus = bus;
	__early_uart_rpi2.base = base;
}

static int early_console_putchar(int c)
{
	__uart_rpi2_putchar(&__early_uart_rpi2, c);
	fbputc(c);
	return 0;
}

static void __jump_handler(void)
{
	__early_console_set_bus(__mapped_bus, __mapped_base);
	set_console(early_console_putchar, DEFAULT_KPUTS);
}

int __early_console_init(struct bus_device *bus, addr_t base, addr_t mapped_base)
{
	__early_console_set_bus(bus, base);
	__mapped_bus = (struct bus_device *) postmap_addr(bus);
	__mapped_base = mapped_base;
	
	__uart_rpi2_init(&__early_uart_rpi2);
	
	set_console(early_console_putchar, PTRCAST(premap_addr(DEFAULT_KPUTS)));

	if (jump_handlers_add(__jump_handler) != 0) while (1);

	return 0;
}

#ifdef RAW /* baremetal driver */

#else /* not RAW, or kernel driver */

#endif /* RAW */

