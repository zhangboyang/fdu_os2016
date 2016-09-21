#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <elf.h>
#include <arch-boot.h>

/*
 * this file is added by ZBY
 */


#define stage0_panic(x) do { __asm__ __volatile__ ("1: jmp 1b"::"a"(x)); } while (0)

#define SECTSIZE 512

// 4096 bytes for elf header should enough
#define BOOT_ELF_PRELOAD 4096

// partition of bootloader ELF, 0 is first partition
#define BOOTLOADER_ELF_PART_ID 0

// all functions should belongs to section .entry
#define __entry __attribute__((section(".entry")))


static unsigned char __entry inb(unsigned short port)
{
    unsigned char data;
    __asm__ __volatile__ ("inb %1, %0": "=a"(data): "d"(port));
    return data;
}
static void __entry outb(unsigned short port, unsigned char data)
{
    __asm__ __volatile__ ("outb %0, %1":: "a"(data), "d"(port));
}
static void __entry insl(int port, void *addr, int cnt)
{
    __asm __volatile__ ("cld; rep insl" :
                        "=D" (addr), "=c" (cnt) :
                        "d" (port), "0" (addr), "1" (cnt) :
                        "memory", "cc");
}



/*
    bootloader stage0:
        on i386, only first sector (512b) will be loaded to 0000:7c00
        stage0 will load entire bootloader ELF to memory and jump to it
    note:
        we can't use .data here
        but we can use .bss (note: .bss hasn't initialized)
*/


static void __entry waitdisk(void)
{
    // Wait for disk ready.
    while ((inb(0x1F7) & 0xC0) != 0x40);
}
static void __entry readsect(void *dst, unsigned int sect)
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

static unsigned int __entry get_elf_sector()
{
    unsigned int *pentry = &mbr[0x1BE + 0x10 * BOOTLOADER_ELF_PART_ID + 0x8];
    return *pentry;
}

void __entry readdisk(void *buf, size_t size, size_t offset)
{   
    // round down to sector
    if (offset % SECTSIZE != 0) stage0_panic(0xEEEE0001);
    offset /= SECTSIZE;
    
    // offset: sector number
    // skip: how many data should we skip
    while (size) {
        // cursize = min(size, SECTSIZE)
        size_t cursize = size < SECTSIZE ? size : SECTSIZE;
    
        // read sector from disk to sector buffer
        static unsigned char sectbuf[SECTSIZE];    
        readsect(buf, offset);

        size -= cursize;
        buf += cursize;
        offset++;
    }
}

void __entry stage0()
{
    static unsigned char elfbuf[BOOT_ELF_PRELOAD];
    
    // read elf header
    struct elfhdr *elf = elfbuf;
    size_t diskoff = get_elf_sector() * SECTSIZE;
    readdisk(elf, BOOT_ELF_PRELOAD, diskoff);
    
    // check elf magic
    if (elf->e_ident[EI_MAG0] != ELFMAG0 ||
        elf->e_ident[EI_MAG1] != ELFMAG1 ||
        elf->e_ident[EI_MAG2] != ELFMAG2 ||
        elf->e_ident[EI_MAG3] != ELFMAG3) stage0_panic(0xEEEE0000);
    
    // read program headers
    struct elf_phdr *ph, *eph;
    ph = (void *) elf + elf->e_phoff;
    eph = ph + elf->e_phnum;
    while (ph < eph) {
        void *pa = (void *) ph->p_paddr;
        if (pa >= hitext) {
            readdisk(pa, ph->p_filesz, diskoff + ph->p_offset);
            if (ph->p_memsz > ph->p_filesz) {
                memset(pa + ph->p_filesz, 0, ph->p_memsz - ph->p_filesz);
            }
        }
        ph++;
    }
    
    bootmain();
    
    stage0_panic(0xEEEE0002);
}

