
#define __entry __attribute__((section(".entry")))

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

void __entry stage0()
{
    struct elfhdr *elf;
    struct elf_phdr *ph, *eph;
    ph = (void *) elf + elf->e_phoff;
    eph = ph + elf->e_phnum;
    while (ph < eph) {
        void *pa = (void *) ph->p_paddr;
        kernreader_readfile(pa, ph->p_filesz, ph->p_offset);
        if (ph->p_memsz > ph->p_filesz) {
            memset(pa + ph->p_filesz, 0, ph->p_memsz - ph->p_filesz);
        }
        ph++;
    }
}

