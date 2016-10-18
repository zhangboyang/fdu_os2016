#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <aim/boot.h>
#include <arch-boot.h>

/*
 * this file is added by ZBY
 */

#define SECTSIZE 512
#define KERNEL_ELF_PART_ID 1



static void readsect(void *dst, unsigned int sect)
{
    if (!readsect_realmode(0x1000, sect)) {
        bpanic("can't read disk in real mode.");
    }
    bmemcpy(dst, (void *) 0x1000, SECTSIZE);
}

static size_t kdiskoffset;

void kernreader_init(void)
{
    // set kdiskoffset
    unsigned char *pentry = &mbr[0x1BE + 0x10 * KERNEL_ELF_PART_ID];
    kdiskoffset = ((pentry[0x8] << 0) +
                   (pentry[0x9] << 8) +
                   (pentry[0xA] << 16) + 
                   (pentry[0xB] << 24)) * SECTSIZE;
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
        readsect(sectbuf, offset);
        
        // copy to buffer
        bmemcpy(buf, sectbuf + skip, cursize - skip);
        
        size -= cursize;
        buf += cursize - skip;
        skip = 0;
        offset++;
    }
}

