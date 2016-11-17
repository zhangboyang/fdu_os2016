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

