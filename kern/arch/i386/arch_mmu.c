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
#include <aim/console.h>

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
        kprintf("pgmid=%x midfn=%x val=%x\n", (unsigned) pgmid, (unsigned) PGMID_FN(va), (unsigned) pgmid[PGMID_FN(va)]);
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

static void check_cpu(void)
{
    kprintf("checking CPUID ... ");
    unsigned eax, ecx, edx, ebx;
    eax = 0x00000001;
    __asm__ __volatile__ ("cpuid":"+eax"(eax),"=ecx"(ecx),"=edx"(edx),"=ebx"(ebx));
    if (!(edx & (1 << 6))) {
        panic("no PAE support in this processor!");
    }
    if (!(edx & (1 << 5))) {
        panic("no RDMSR/WRMSR in this processor!");
    }
    eax = 0x80000001;
    __asm__ __volatile__ ("cpuid":"+eax"(eax),"=ecx"(ecx),"=edx"(edx),"=ebx"(ebx));
    if (!(edx & (1 << 20))) {
        panic("no NX/XD support in this processor!");
    }
    kprintf("OK\n");
}

#define MAX_MACHINE_MEMORY_MAP 20
struct machine_memory_map {
    uint64_t base, size;
    uint32_t type, unknown;
};
struct machine_memory_map mach_mem_map[MAX_MACHINE_MEMORY_MAP];
unsigned nr_mach_mem_map;

static void copy_memory_map()
{
    // read memory map from bootloader
    if (memcmp((void *) 0x10000, "_AIM_MEMORY_MAP", 16) != 0) {
        panic("no memory map at 0x10000!");
    }
    nr_mach_mem_map = *(unsigned *) (0x10000 + 16);
    if (nr_mach_mem_map > MAX_MACHINE_MEMORY_MAP) {
        panic("too many memory regions!");
    }
    memcpy(mach_mem_map, (void *) (0x10000 + 0x24), sizeof(struct machine_memory_map) * nr_mach_mem_map);

    // print memory map to console    
    int i;
    uint64_t total = 0;
    kprintf("AIM kernel memory map: (%d regions)\n", nr_mach_mem_map);
    for (i = 0; i < nr_mach_mem_map; i++) {
        struct machine_memory_map *r = &mach_mem_map[i];
        if (r->type == 1) {
            total += r->size;
        }
        kprintf("  base %08x %08x size %08x %08x end %08x %08x type %d\n", (unsigned)(r->base >> 32), (unsigned)(r->base), (unsigned)(r->size >> 32), (unsigned)(r->size), (unsigned)((r->base + r->size) >> 32), (unsigned)(r->base + r->size), r->type);
    }
    kprintf("total memory size: %d MB\n\n", (total >> 20));
}

void early_mm_init(void)
{
    check_cpu();
    copy_memory_map();
    
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
        "or $0x00000020, %%eax\n\t"
        "mov %%eax, %%cr4\n\t"
        :::"eax"
    );
    
    // enable NX/XD
    __asm__ __volatile__ (
        "mov $0xC0000080, %%ecx\n\t" // IA32_EFER
        "rdmsr\n\t"
        "orl $(1 << 11), %%eax\n\t" // IA32_EFER.NXE
        "wrmsr\n\t"
        :::"eax", "ecx", "edx"
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
