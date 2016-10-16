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


// make macros in PREMAP mode in 'mmu.h'
#define PREMAP


#include <sys/types.h>
#include <util.h>
#include <aim/debug.h>
#include <aim/mmu.h>
#include <aim/early_kmmap.h>
#include <libc/string.h>
#include <libc/stdio.h>
#include <aim/init.h>

/*
   ZBY: use PAE for i386
*/



static __attribute((aligned(PGINDEX_ALIGN))) pgindex_t __boot_page_index[PGINDEX_SIZE];

static __attribute((aligned(PGMID_ALIGN))) pgmid_t __boot_page_mid_all[PGINDEX_SIZE][PGMID_SIZE];


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
        pgmid[PGMID_FN(va)] = MKPGMID_BIG(pa) | PGMID_RW;
        bprintf("pgmid=%x midfn=%x val=%x\n", (unsigned) pgmid, (unsigned) PGMID_FN(va), (unsigned) pgmid[PGMID_FN(va)]);
    }
    
    return 0;
}


/*
    ZBY: instead clearing the @boot_page_index
         we initialize the @boot_page_index using kernel .bss memory
         only init first two level of pgindex
         we will use 2MB pages later
*/
void page_index_clear(pgindex_t *boot_page_index)
{
    int i;
    
    // init the first level of pgindex
    for (i = 0; i < PGINDEX_SIZE; i++) {
        __boot_page_index[i] = MKPGINDEX(__boot_page_mid_all[i]);
    }
    
    // clear the second level of pgindex (clear P bit to 0)
    memset(__boot_page_mid_all, 0, sizeof(__boot_page_mid_all));
}


// check alignment of struct early_mapping
// see 'early_kmmap.c' for details
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



void early_mm_init(void)
{
    early_mapping_clear();
    
    addr_t ksize = PTR2ADDR(KERN_END_HIGH) - KOFFSET;
    struct early_mapping entry;
    
    entry = (struct early_mapping) {
		.paddr	= 0,
		.vaddr	= 0,
		.size	= ROUNDUP(ksize, BIGPAGE_SIZE),
		.type	= EARLY_MAPPING_MEMORY
	};
	early_mapping_add(&entry);
	
	entry = (struct early_mapping) {
		.paddr	= 0,
		.vaddr	= ADDR2PTR(KOFFSET),
		.size	= ROUNDUP(ksize, BIGPAGE_SIZE),
		.type	= EARLY_MAPPING_MEMORY
	};
	early_mapping_add(&entry);
	
    mmu_init(__boot_page_index);
}

void mmu_jump()
{
    // enable PAE
    __asm__ __volatile__ (
        "mov %%cr4, %%eax\n\t"
        "or $0x00000010, %%eax\n\t"
        "mov %%eax, %%cr4\n\t"
        :::"eax"
    );
    
    // move page table to cr3
    __asm__ __volatile__ ("mov %0, %%cr3\n\t"::"r"(__boot_page_index));
    
    // enable pageing
    __asm__ __volatile__ (
        "mov %%cr0, %%eax\n\t"
        "or $0x80000000, %%eax\n\t"
        "mov %%eax, %%cr0\n\t"
        :::"eax"
    );
    abs_jump(master_entry);
}


__asm__ (
    ".globl abs_jump\n"
    "abs_jump:\n"
    "   pop %eax\n"
    "   ret\n"
);
