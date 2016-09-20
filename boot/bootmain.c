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

static inline void *boot_memset(void *s, int c, size_t n)
{
    unsigned char *p = s;
    while (n--) *p++ = c;
    return s;
}

__noreturn
void bootmain(void)
{
    static unsigned char elfbuf[BOOT_ELF_PRELOAD];
    struct elfhdr *elf = (void *) elfbuf;

    // init elfreader
    kernreader_init();
    
    // read first BOOT_ELF_PRELOAD bytes to buffer
    kernreader_readfile(elf, BOOT_ELF_PRELOAD, 0);

    // check elf magic
    if (elf->magic != ELF_MAGIC) boot_panic();

    // load each program segment (ignores ph flags)
    struct proghdr *ph, *eph;
    ph = (void *) elf + elf->phoff;
    eph = ph + elf->phnum;
    while (ph < eph) {
        void *pa = (void *) ph->paddr;
        kernreader_readfile(pa, ph->filesz, ph->off);
        if (ph->memsz > ph->filesz) {
            boot_memset(pa + ph->filesz, 0, ph->memsz - ph->filesz);
        }
        ph++;
    }

    // call entry point, should not return
    (void (*)(void)) (elf->entry) ();
    
    boot_panic();
}

