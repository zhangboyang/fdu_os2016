/*
 * this file is added by ZBY
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

static inline unsigned char inb(unsigned short port)
{
    unsigned char data;
    __asm__ __volatile__ ("inb %1, %0": "=a"(data): "d"(port));
    return data;
}
static inline void outb(unsigned short port, unsigned char data)
{
    __asm__ __volatile__ ("outb %0, %1":: "a"(data), "d"(port));
}
static inline void insl(int port, void *addr, int cnt)
{
    __asm __volatile__ ("cld; rep insl" :
                        "=D" (addr), "=c" (cnt) :
                        "d" (port), "0" (addr), "1" (cnt) :
                        "memory", "cc");
}

static void waitdisk(void)
{
    // Wait for disk ready.
    while ((inb(0x1F7) & 0xC0) != 0x40);
}
static void readsect(void *dst, unsigned int sect)
{
    // Issue command.
    waitdisk();
    outb(0x1F2, 1);   // count = 1
    outb(0x1F3, sect);
    outb(0x1F4, sect >> 8);
    outb(0x1F5, sect >> 16);
    outb(0x1F6, (sect >> 24) | 0xE0);
    outb(0x1F7, 0x20);  // cmd 0x20 - read sectors

    // Read data.
    waitdisk();
    insl(0x1F0, dst, SECTSIZE / 4);
}

static size_t kdiskoffset;

void kernreader_init(void)
{
    // set kdiskoffset
    kdiskoffset = 206848 * 512; // FIXME, should read from MBR
}

void kernreader_readfile(void *buf, size_t size, size_t offset)
{
    // translate fileoffset to diskoffset
    offset += kdiskoffset;
    
    // round down to sector
    size_t skip = offset % SECTSIZE;
    offset /= SECTSIZE;
    size += skip;
    
    // offset: sector number
    // skip: how many data should we skip
    while (size) {
        // cursize = min(size, SECTSIZE)
        size_t cursize = size < SECTSIZE ? size : SECTSIZE;
    
        // read sector from disk to sector buffer
        static unsigned char sectbuf[SECTSIZE];    
        readsect(sectbuf, sect);
        
        // copy to buffer
        memcpy(buf, sectbuf + skip, cursize - skip);
        
        size -= cursize;
        buf += cursize - skip;
        skip = 0;
        sect++;
    }
}

