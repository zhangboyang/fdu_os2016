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
int ask_property(void *buf, size_t reqsize, size_t bufsize);


#define MAILBOX_PROP_ARMMEMORY 0x00010005

#define PROPERTY_TAG_REQUEST 0
#define PROPERTY_TAG_RESPONSE 1

struct property_tag {
    uint32_t id; // tag identifier
    uint32_t bufsize; // value buffer size in bytes
    uint32_t size : 31;
    uint32_t type : 1; // 1 bit (MSB) request/response indicator (0=request, 1=response), 31 bits (LSB) value length in bytes
    union {
        uint8_t val8[0];
        uint32_t val32[0];
    };
};

    

#endif /* _MACH_PLATFORM_H */

