/* Copyright (C) 2016 David Gao <davidgao1001@gmail.com>
 *
 * This file is part of AIMv6.
 *
 * AIMv6 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * AIMv6 is distributed in the hope that it will be useful,
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



#define BOOT_ELF_PRELOAD 4096

__noreturn
void bootmain(void)
{
    bioinit();
    bputs("welcome to ZBY's bootloader for AIM");
    bputs(
        "           _____ __  __         __  \n"
        "     /\\   |_   _|  \\/  |       / /  \n"
        "    /  \\    | | | \\  / |_   __/ /_  \n"
        "   / /\\ \\   | | | |\\/| \\ \\ / / '_ \\ \n"
        "  / ____ \\ _| |_| |  | |\\ V /| (_) |\n"
        " /_/    \\_\\_____|_|  |_| \\_/  \\___/ \n"
    );

    static unsigned char elfbuf[BOOT_ELF_PRELOAD];
    struct elfhdr *elf = (void *) elfbuf;

    // init elfreader
    kernreader_init();
    
    // read first BOOT_ELF_PRELOAD bytes to buffer
    kernreader_readfile(elf, BOOT_ELF_PRELOAD, 0);

    // check elf magic
    if (elf->e_ident[EI_MAG0] != ELFMAG0 ||
        elf->e_ident[EI_MAG1] != ELFMAG1 ||
        elf->e_ident[EI_MAG2] != ELFMAG2 ||
        elf->e_ident[EI_MAG3] != ELFMAG3) boot_panic("ELF magic mismatch!");

    // load each program segment (ignores ph flags)
    struct elf_phdr *ph, *eph;
    ph = (void *) elf + elf->e_phoff;
    eph = ph + elf->e_phnum;
    while (ph < eph) {
        void *pa = (void *) ph->p_paddr;
        boot_panic("ahaa");
        bprintf("phdr type=%x flags=%x offset=%x\n", (unsigned) ph->p_type, (unsigned) ph->p_flags, (unsigned) ph->p_offset);
        bprintf("     vaddr=%p paddr=%p\n", (void *) ph->p_vaddr, (void *) ph->p_paddr);
        bprintf("     filesz=%x memsz=%x align=%x\n", (unsigned) ph->p_filesz, (unsigned) ph->p_memsz, (unsigned) ph->p_align);
        boot_panic("aha");
        kernreader_readfile(pa, ph->p_filesz, ph->p_offset);
        if (ph->p_memsz > ph->p_filesz) {
            memset(pa + ph->p_filesz, 0, ph->p_memsz - ph->p_filesz);
        }
        ph++;
    }

    // call entry point, should not return
    void (*entry) (void) __attribute__((noreturn));
    entry = (void *) elf->e_entry;
    entry();
}

