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


#define KVA2PA(x) (PTR2ADDR(x))
#define PA2KVA(x) (ADDR2PTR(x))


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
        //kprintf("pgmid=%x midfn=%x val=%x\n", (unsigned) pgmid, (unsigned) PGMID_FN(va), (unsigned) pgmid[PGMID_FN(va)]);
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





struct machine_memory_map mach_mem_map[MAX_MACHINE_MEMORY_MAP];
unsigned nr_mach_mem_map;
size_t size_after_kernel;

static void copy_memory_map()
{
    // read memory map from bootloader
    if (memcmp((void *) 0x10000, "_AIM_MEMORY_MAP", 16) != 0) {
        panic("no memory map at 0x10000!");
    }
    unsigned cnt = *(unsigned *) (0x10000 + 16);
    if (cnt > MAX_MACHINE_MEMORY_MAP) {
        panic("too many memory regions!");
    }
    memcpy(LOWADDR(mach_mem_map), (void *) (0x10000 + 24), sizeof(struct machine_memory_map) * cnt);

    // print memory map to console    
    unsigned i;
    uint64_t total = 0;
    kprintf("AIM kernel memory map: (%d regions)\n", cnt);
    for (i = 0; i < cnt; i++) {
        struct machine_memory_map *r = LOWADDR(&mach_mem_map[i]);
        if (r->type == 1) {
            total += r->size;
        }
        kprintf("  base %08x %08x size %08x %08x end %08x %08x type %d\n", (unsigned)(r->base >> 32), (unsigned)(r->base), (unsigned)(r->size >> 32), (unsigned)(r->size), (unsigned)((r->base + r->size) >> 32), (unsigned)(r->base + r->size), r->type);
    }
    kprintf("total memory size: %d MB\n", (total >> 20));
    
    
    // calc size_after_kernel
    size_t freemem = 0;
    for (i = 0; i < cnt; i++) {
        struct machine_memory_map *r = LOWADDR(&mach_mem_map[i]);
        if (r->type == 1) {
            if (r->base <= (uint64_t) ULCAST(KERN_START_LOW) && r->base + r->size >= (uint64_t) ULCAST(KERN_END_LOW)) {
                freemem = (r->base + r->size) - (uint64_t) ULCAST(KERN_END_LOW);
            }
        }
    }
    freemem = min2(KTOP - KOFFSET, freemem);
    
    if (freemem) {
        kprintf("free memory after kernel: %d MB\n", (freemem >> 20));
    } else {
        panic("no free memory after kernel!");
    }
    
    *LOWADDR(&size_after_kernel) = freemem;
    *LOWADDR(&nr_mach_mem_map) = cnt;
    kprintf("\n");
}

void early_mm_init(void)
{
    check_cpu();
    copy_memory_map();
    
    addr_t ksize = KTOP - KOFFSET;
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


__asm__ ( // the abs_jump()
    ".globl abs_jump\n"
    "abs_jump:\n"
    "   pop %eax\n"
    "   ret\n"
);



////////////////////////// HIGHADDR FUNCTIONS ///////////////////////////////
#undef KVA2PA
#undef PA2KVA

#include <aim/pmm.h>



//////////// vmm and pmm related functions
void arch_init_pmm_zone()
{
    // query memory zones
    lsize_t normal_top = 0, max_physmem = 0;
    for (size_t i = 0; i < nr_mach_mem_map; i++) {
        struct machine_memory_map *r = &mach_mem_map[i];
        if (r->type == 1) { // useable memory
            if (r->base <= (uint64_t) ULCAST(KERN_START_LOW) && r->base + r->size >= (uint64_t) ULCAST(KERN_END_LOW)) {
                normal_top = (r->base + r->size);
            }
        }
        max_physmem = max2(max_physmem, (r->base + r->size));
    }
    normal_top = min2(normal_top, KTOP - KOFFSET);
    
    normal_top = ROUNDDOWN(normal_top, PAGE_SIZE);
    max_physmem = ROUNDDOWN(max_physmem, PAGE_SIZE);
    

    // init each zone
    assert(IS_ALIGNED(normal_top, PAGE_SIZE));
    assert(IS_ALIGNED(max_physmem, PAGE_SIZE));
    assert(max_physmem >= normal_top);
    assert(MAX_MEMORY_ZONE == 3);
    pmm_zone[ZONE_DMA] = (struct zone) { // DMA zone, no need to query memory map
        .base = 0, .size = 0x1000000, // first 16 MB of memory
        .page_size = PAGE_SIZE,
    };
    pmm_zone[ZONE_NORMAL] = (struct zone) {
        .base = 0x1000000, .size = (normal_top - 0x1000000),
        .page_size = PAGE_SIZE,
    };
    pmm_zone[ZONE_HIGHMEM] = (struct zone) {
        .base = normal_top, .size = (max_physmem - normal_top),
        .page_size = PAGE_SIZE,
    };
    
    // print summary to console
    kprintf("physical memory zone summary:\n");
    const char *zone_name[] = { "DMA", "NORMAL", "HIGHMEM" };
    for (int i = 0; i < MAX_MEMORY_ZONE; i++) {
        kprintf(" zone %10s base %016llx size %016llx end %016llx pagesz %x, %d MB\n", zone_name[i], pmm_zone[i].base, pmm_zone[i].size, pmm_zone[i].base + pmm_zone[i].size, pmm_zone[i].page_size, (int)((pmm_zone[i].base + pmm_zone[i].size) >> 20));
    }
}

void try_free_page(int zone, addr_t page)
{
    if (pmm_zone[zone].base <= page && page < pmm_zone[zone].base + pmm_zone[zone].size) {
        VF(pmm_zone[zone].allocator, free, page);
    }
}

void arch_init_free_pmm_zone(addr_t kstart, addr_t kend)
{
    // query memory zones
    kprintf("memory from %08x to %08x belongs to kernel\n", (unsigned) kstart, (unsigned) kend);
    for (size_t i = 0; i < nr_mach_mem_map; i++) {
        struct machine_memory_map *r = &mach_mem_map[i];
        if (r->type == 1) { // useable memory
            addr_t st = r->base, ed = r->base + r->size;
            st = ROUNDUP(st, PAGE_SIZE);
            ed = ROUNDDOWN(ed, PAGE_SIZE);
            kprintf("free memory from %016llx to %016llx, %lld KB\n", st, ed, ((ed - st) >> 10));
            for (addr_t page = st; page < ed; page += PAGE_SIZE) {
                // try free 'page'
                if (kstart <= page && page < kend) continue;
                for (int i = 0; i < MAX_MEMORY_ZONE; i++) {
                    try_free_page(i, page);
                }
            }
        }
    }
}


////////////////// cpu related functions



struct cpu *cpu;
struct int nr_cpu;

void cpu_init_structure()
{
    nr_cpu = 1;
    
}
