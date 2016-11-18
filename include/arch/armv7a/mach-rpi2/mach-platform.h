#ifndef _MACH_PLATFORM_H
#define _MACH_PLATFORM_H


#define RPI2_MEMIO_BASE  0x3F000000

// videocore physical memory offset
#define RPI2_VC_POFFSET 0x40000000

// physical address to video core address
#define PA2VCA(addr) (addr + RPI2_VC_POFFSET)



/* for UART */
#define UART_BASE RPI2_MEMIO_BASE 

#define EARLY_CONSOLE_BUS	(&early_memory_bus)
#define EARLY_CONSOLE_BASE	UART_BASE
#define EARLY_CONSOLE_MAPPING	MAP_LINEAR

/* mailbox api */
int write_mailbox(uint32_t channel, uint32_t data);
int read_mailbox(uint32_t channel, uint32_t *data);
void mailbox_init();

#endif /* _MACH_PLATFORM_H */

