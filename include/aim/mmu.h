/* Copyright (C) 2016 David Gao <davidgao1001@gmail.com>
 * Copyright (C) 2016 Gan Quan <coin2028@hotmail.com>
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

#ifndef _AIM_MMU_H
#define _AIM_MMU_H

#include <arch-mmu.h>
#include <util.h>

#define KOFFSET KERN_BASE
#define KTOP 0xFE000000
#define LOWADDR(highaddr) ((typeof((highaddr) + 0)) ((ULCAST(highaddr)) - KERN_BASE))
/* addresses before and after early MMU mapping */
#define __premap_addr(kva)	(ULCAST(kva) - KERN_BASE)
#define __postmap_addr(pa)	(ULCAST(pa) + KERN_BASE)
/* kernel virtual address and physical address conversion */
#define kva2pa(kva)		(ULCAST(kva) - KERN_BASE)
#define pa2kva(pa)		(PTRCAST(pa) + KERN_BASE)
#define VMA2LMA(x) ((x) - KOFFSET)


#ifndef PAGE_SIZE
#define PAGE_SIZE	4096
#endif /* !PAGE_SIZE */

#ifndef EARLY_PAGE_SIZE
#define EARLY_PAGE_SIZE	PAGE_SIZE
#endif /* !EARLY_PAGE_SIZE */

#ifndef __ASSEMBLER__


/* premap_addr: always returns low address.
 * The function which assumes that the argument is a high address
 * becomes __premap_addr(). */
#define premap_addr(a)	({ \
	size_t ___i = (size_t)(a); \
	(___i >= KERN_BASE) ? __premap_addr(___i) : ___i; \
})

/* postmap_addr: always returns high address.
 * The function which assumes that the argument is a low address
 * becomes __postmap_addr(). */
#define postmap_addr(a)	({ \
	size_t ___i = (size_t)(a); \
	(___i >= KERN_BASE) ? (___i) : __postmap_addr(___i); \
})


addr_t get_mem_physbase();
addr_t get_mem_size();

void page_index_clear(pgindex_t *boot_page_index);
int page_index_early_map(pgindex_t *boot_page_index, addr_t paddr,
	void *vaddr, size_t size);
int page_index_init(pgindex_t *boot_page_index);
void mmu_init(pgindex_t *boot_page_index);

void early_mm_init(void);	/* arch-specific */

void mm_init(void);
void arch_mm_init(void);	/* arch-specific */









/* Clear all MMU init callback handlers */
void mmu_handlers_clear(void);
/* Add one MMU init callback handler, which will be called *after*
 * initializing MMU*/
int mmu_handlers_add(generic_fp entry);
void mmu_handlers_apply(void);

/* Likewise */
void jump_handlers_clear(void);
int jump_handlers_add(generic_fp entry);
void jump_handlers_apply(void);







/*
 * This routine jumps to an absolute address, regardless of MMU and page index
 * state.
 * by jumping to some address, callers acknowledge that C runtime components
 * like stack are not preserved, and no return-like operation will be performed.
 */
__noreturn
void abs_jump(void *addr);


// jump to high address
__noreturn
void mmu_jump();





/*
 * Architecture-specific interfaces
 * All addresses should be page-aligned.
 * Note that these interfaces are independent of struct mm and struct vma.
 * Also, these interfaces assume that the page index is exclusively owned.
 */
/* Initialize a page index table and fill in the structure @pgindex */
pgindex_t *init_pgindex(void);
/* Destroy the page index table itself assuming that everything underlying is
 * already done with */
void destroy_pgindex(pgindex_t *pgindex);






/* Switch page index to the given one */
int switch_pgindex(pgindex_t *pgindex);
/* Get the currently loaded page index structure */
pgindex_t *get_pgindex(void);








/* Map virtual address starting at @vaddr to physical pages at @paddr, with
 * VMA flags @flags (VMA_READ, etc.) Additional flags apply as in kmmap.h.
 */
int map_pages(pgindex_t *pgindex, void *vaddr, addr_t paddr, size_t size,
    uint32_t flags);
/*
 * Unmap but do not free the physical frames
 * Returns the size of unmapped physical memory (may not equal to @size), or
 * negative for error.
 * The physical address of unmapped pages are stored in @paddr.
 */
ssize_t unmap_pages(pgindex_t *pgindex, void *vaddr, size_t size, addr_t *paddr);
/*
 * Change the page permission flags without changing the actual mapping.
 */
int set_pages_perm(pgindex_t *pgindex, void *vaddr, size_t size, uint32_t flags);
/*
 * Invalidate pages, but without actually deleting the page index entries, 
 * if possible.
 */
ssize_t invalidate_pages(pgindex_t *pgindex, void *vaddr, size_t size,
    addr_t *paddr);








/* Trace a page index to convert from user address to kernel address */
//void *uva2kva(pgindex_t *pgindex, void *uaddr);








//////////////////////////// ZBY ////////////////////////////////////////////

extern void arch_init_pmm_zone();
extern void arch_init_free_pmm_zone(addr_t kstart, addr_t kend);

extern void cpu_init_structure();
extern void cpu_reload(struct cpu *cpu);
extern void cpu_reload_segments(); // in entry.S
extern struct cpu *cpu_list;
extern int nr_cpu;

extern uint8_t KERN_START_HIGH[];
extern uint8_t KERN_END_HIGH[];
extern uint8_t KERN_START_LOW[];
extern uint8_t KERN_END_LOW[];
extern size_t size_after_kernel; // how many bytes can we use after the kernel


#define PTR2ADDR(x) ((addr_t)(uint32_t)(x))
#define ADDR2PTR(x) ((void *)(unsigned long)(x))

#define PGTABLE_SHIFT   PGOFFSET_BITS                       // 12
#define PGMID_SHIFT     (PGTABLE_SHIFT + PGTABLE_BITS)      // 12 + 9 = 21
#define PGINDEX_SHIFT   (PGMID_SHIFT + PGMID_BITS)          // 21 + 9 = 30

#define PGINDEX_SIZE    (1ULL << PGINDEX_BITS)     // 4
#define PGMID_SIZE      (1ULL << PGMID_BITS)       // 512
#define PGTABLE_SIZE    (1ULL << PGTABLE_BITS)     // 512


#define PGINDEX_ALIGN   (sizeof(pgindex_t) * PGINDEX_SIZE)        // 32
#define PGMID_ALIGN     (sizeof(pgmid_t) * PGMID_SIZE)            // 4096
#define PGTABLE_ALIGN   (sizeof(pgtable_t) * PGTABLE_SIZE)        // 4096



#define PAGE_SIZE       (1ULL << PGOFFSET_BITS)
#define BIGPAGE_SIZE    (1ULL << PGBIGOFFSET_BITS)


// FN means frame number
#define PGINDEX_FN(x)   (((x) >> PGINDEX_SHIFT) & ((1ULL << PGINDEX_BITS) - 1))
#define PGMID_FN(x)     (((x) >> PGMID_SHIFT) & ((1ULL << PGMID_BITS) - 1))
#define PGTABLE_FN(x)   (((x) >> PGTABLE_SHIFT) & ((1ULL << PGTABLE_BITS) - 1))
#define PAGE_FN(x)      ((x) & ~(PAGE_SIZE - 1))
#define BIGPAGE_FN(x)   ((x) & ~(BIGPAGE_SIZE - 1))




#define PAGE_OFF(x)     ((x) & (PAGE_SIZE - 1))
#define BIGPAGE_OFF(x)  ((x) & (BIGPAGE_SIZE - 1))
#define ROUNDTO_PAGE(x)     (PAGE_OFF(x))
#define ROUNDTO_BIGPAGE(x)  (BIGPAGE_OFF(x))

// walk a pgindex_t (i.e. get the VA in pgindex_t)
#define WKPGINDEX(pgindex)  (PA2KVA(PAGE_FN(pgindex)))


#endif /* !__ASSEMBLER__ */

#endif /* _AIM_MMU_H */

