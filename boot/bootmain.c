/* Copyright (C) 2016 David Gao <davidgao1001@gmail.com>
 *
 * This file is part of AIM.
 *
 * AIM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * AIM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <aim/boot.h>
#include <elf.h>



#define BOOT_ELF_PRELOAD 4096

__noreturn
void bootmain(void)
{
    /////////////////// INIT I/O //////////////////////////////////////////
    bioinit();
    bputs("welcome to ZBY's bootloader for AIM");
    bputs(
        "           _____ __  __\n"
        "     /\\   |_   _|  \\/  |\n"
        "    /  \\    | | | \\  / |\n"
        "   / /\\ \\   | | | |\\/| |\n"
        "  / ____ \\ _| |_| |  | |\n"
        " /_/    \\_\\_____|_|  |_|\n"
    );

    bprintf("bootloader itself is located from %x to %x, length %x\n\n", text_begin, text_end, text_end - text_begin);
    
    
    /////////////////// DETECT MEMORY /////////////////////////////////////
    detectmemory();
    
    
    /////////////////// LOAD ELF //////////////////////////////////////////
    static unsigned char elfbuf[BOOT_ELF_PRELOAD];
    elf_hdr *elf = (void *) elfbuf;

    // init elfreader
    kernreader_init();
    
    // read first BOOT_ELF_PRELOAD bytes to buffer
    bprintf("read ELF header ... ");
    kernreader_readfile(elf, BOOT_ELF_PRELOAD, 0);
    bputc('\n');

    // check elf magic
    if (elf->e_ident[EI_MAG0] != ELFMAG0 ||
        elf->e_ident[EI_MAG1] != ELFMAG1 ||
        elf->e_ident[EI_MAG2] != ELFMAG2 ||
        elf->e_ident[EI_MAG3] != ELFMAG3) bpanic("ELF magic mismatch!");

    // load each program segment (ignores ph flags)
    elf_phdr *ph, *eph;
    ph = (void *) elf + elf->e_phoff;
    eph = ph + elf->e_phnum;
    
    bprintf("there are %d program headers in kernel ELF.\n", (int) elf->e_phnum);
    if (elf->e_phnum == 0) bpanic("no program headers in ELF file!");
    
    void *entry;
    entry = (void *) elf->e_entry;
    
    while (ph < eph) {
        bprintf("phdr type=%x flags=%x offset=%x\n", (unsigned) ph->p_type, (unsigned) ph->p_flags, (unsigned) ph->p_offset);
        bprintf("     vaddr=%p paddr=%p\n", (void *) ph->p_vaddr, (void *) ph->p_paddr);
        bprintf("     filesz=%x memsz=%x align=%x\n", (unsigned) ph->p_filesz, (unsigned) ph->p_memsz, (unsigned) ph->p_align);
        
        // load
        void *pa = (void *) ph->p_paddr;
        kernreader_readfile(pa, ph->p_filesz, ph->p_offset);
        if (ph->p_memsz > ph->p_filesz) {
            bmemset(pa + ph->p_filesz, 0, ph->p_memsz - ph->p_filesz);
        }
        
        // adjust entry if necessary
        void *va = (void *) ph->p_vaddr;
        if (va <= entry && entry < va + ph->p_memsz) {
            entry = entry - ph->p_vaddr + ph->p_paddr;
        }
        
        bputc('\n');
        ph++;
    }

    

    //////////////// JUMP TO KERNEL //////////////////////////////////////    
    #if defined(__i386__)
    /* FIXME: THIS IS THE SUPER DIRTY bprintf() HACK
        currently, there is no printf() in kernel
        fortunately, the loader bootloader has it own bprintf()
        so we make a 'JMP bprintf' at 0x7c00
        thus, we can use
            #define bprintf ((int (*)(const char *fmt, ...)) 0x7c00)
        to call bprintf() in kernel
    */
    unsigned joffset = (unsigned) bprintf - (0x7c00 + 5);
    mbr[0] = '\xE9'; // opcode of JMP
    bmemcpy(&mbr[1], &joffset, 4);
    #elif defined(__armv7__)
    sfdasfs
    #endif

    // call entry point, should not return
    bprintf("\nkernel entry: %p\n", entry);
    bputs("jump to kernel ...\n");

    ((void (*)(void)) entry)();
    
    bpanic("kernel entry returned!");
}

