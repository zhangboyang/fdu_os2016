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
#include <aim/early_kmmap.h>
#include <aim/mmu.h>
#include <libc/string.h>
#include <aim/debug.h>


#define KVA2PA(x) (PTR2ADDR(x))
#define PA2KVA(x) (ADDR2PTR(x))


static __attribute((aligned(PGINDEX_ALIGN))) pgindex_t __boot_page_index[PGINDEX_SIZE];
static __attribute((aligned(PGMID_ALIGN))) pgmid_t __boot_page_mid_all[PGINDEX_SIZE][PGMID_SIZE];


bool early_mapping_valid(struct early_mapping *entry)
{
    // we use 2MB pages in early mapping
    return BIGPAGE_OFF(entry->paddr) == 0 &&
           BIGPAGE_OFF(PTR2ADDR(entry->vaddr)) == 0 &&
           BIGPAGE_OFF(entry->size) == 0;
}

void mmu_init(pgindex_t *boot_page_index)
{
    page_index_init(boot_page_index);
}


void page_index_clear(pgindex_t *boot_page_index)
{
    // init the first level of pgindex
    for (int i = 0; i < PGINDEX_SIZE; i++) {
        __boot_page_index[i] = MKPGINDEX(__boot_page_mid_all[i]);
    }
    
    // clear the second level of pgindex (clear P bit to 0)
    memset(__boot_page_mid_all, 0, sizeof(__boot_page_mid_all));
}

int page_index_early_map(pgindex_t *boot_page_index, addr_t paddr, void *vaddr, size_t size)
{
    // assert for alignment
    assert(BIGPAGE_OFF(paddr) == 0);
    assert(BIGPAGE_OFF(PTR2ADDR(vaddr)) == 0);
    assert(BIGPAGE_OFF(size) == 0);
    
    addr_t pa, va;
    for (pa = paddr, va = PTR2ADDR(vaddr); pa < paddr + size; pa += BIGPAGE_SIZE, va += BIGPAGE_SIZE) {
        // map VA to PA
        pgmid_t *pgmid = WKPGINDEX(boot_page_index[PGINDEX_FN(va)]);
        pgmid[PGMID_FN(va)] = MKPGMID_BIG(pa);
        kprintf("pgmid=%llx midfn=%llx val=%llx\n", ULLCAST(ULCAST(pgmid)), ULLCAST(PGMID_FN(va)), ULLCAST(pgmid[PGMID_FN(va)]));
    }
    
    // FIXME: device memory attr
    
    return 0;
}

void early_mm_init(void)
{
    // initialize high address
    
    init_jmphigh_mapping();
	
    mmu_init(__boot_page_index);
}

void mmu_jump()
{
    while (1);
}


#undef KVA2PA
#undef PA2KVA
