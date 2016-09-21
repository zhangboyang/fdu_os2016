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

#define boot_panic() while (1)
#define BOOT_ELF_PRELOAD 4096

__noreturn
void bootmain(void)
{
    bioinit();
    bputs("Hello World! We are now at Stage1");
    while (1);
    static unsigned char elfbuf[BOOT_ELF_PRELOAD];
    struct elfhdr *elf = (void *) elfbuf;

    // init elfreader
    kernreader_init();
    
    // read first BOOT_ELF_PRELOAD bytes to buffer
    kernreader_readfile(elf, BOOT_ELF_PRELOAD, 0);

    /*// check elf magic
    if (elf->e_ident[EI_MAG0] != ELFMAG0 ||
        elf->e_ident[EI_MAG1] != ELFMAG1 ||
        elf->e_ident[EI_MAG2] != ELFMAG2 ||
        elf->e_ident[EI_MAG3] != ELFMAG3) boot_panic();*/

    // load each program segment (ignores ph flags)
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

    // call entry point, should not return
    void (*entry) (void) __attribute__((noreturn));
    entry = (void *) elf->e_entry;
    entry();
}

