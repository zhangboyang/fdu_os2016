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

#ifndef _ARCH_MMU_H
#define _ARCH_MMU_H


/* addresses before and after early MMU mapping */
#define __premap_addr(kva)	(ULCAST(kva) - KOFFSET)
#define __postmap_addr(pa)	(ULCAST(pa) + KOFFSET)


/*
    ZBY: some macros copied from xv6
*/

//
// assembler macros to create x86 segments
//

#define SEG_NULLASM                                             \
        .word 0, 0;                                             \
        .byte 0, 0, 0, 0

// The 0xC0 means the limit is in 4096-byte units
// and (for executable segments) 32-bit mode.
#define SEG_ASM(type,base,lim)                                  \
        .word (((lim) >> 12) & 0xffff), ((base) & 0xffff);      \
        .byte (((base) >> 16) & 0xff), (0x90 | (type)),         \
                (0xC0 | (((lim) >> 28) & 0xf)), (((base) >> 24) & 0xff)

#define STA_X     0x8       // Executable segment
#define STA_E     0x4       // Expand down (non-executable segments)
#define STA_C     0x4       // Conforming code segment (executable only)
#define STA_W     0x2       // Writeable (non-executable segments)
#define STA_R     0x2       // Readable (executable segments)
#define STA_A     0x1       // Accessed



// various segment selectors.
#define SEG_KCODE 1  // kernel code
#define SEG_KDATA 2  // kernel data+stack
#define SEG_KCPU  3  // kernel per-cpu data
#define SEG_UCODE 4  // user code
#define SEG_UDATA 5  // user data+stack
#define SEG_TSS   6  // this process's task state


// Control Register flags
#define CR0_PE          0x00000001      // Protection Enable
#define CR0_MP          0x00000002      // Monitor coProcessor
#define CR0_EM          0x00000004      // Emulation
#define CR0_TS          0x00000008      // Task Switched
#define CR0_ET          0x00000010      // Extension Type
#define CR0_NE          0x00000020      // Numeric Errror
#define CR0_WP          0x00010000      // Write Protect
#define CR0_AM          0x00040000      // Alignment Mask
#define CR0_NW          0x20000000      // Not Writethrough
#define CR0_CD          0x40000000      // Cache Disable
#define CR0_PG          0x80000000      // Paging



#define CR4_PAE         (1 << 5)


#define KOFFSET 0xC0000000
#define VMA2LMA(x) ((x) - KOFFSET)

#ifndef __ASSEMBLER__

extern uint8_t KERN_START_HIGH[];
extern uint8_t KERN_END_HIGH[];


/*
   ZBY: use PAE for i386
*/


typedef uint64_t pdpte_t; // level 3
typedef uint64_t pde_t; // level 2
typedef uint64_t pte_t; // level 1

typedef pdpte_t pgindex_t;
typedef pde_t pgmid_t;
typedef pte_t pgtable_t;





#define PGINDEX_BITS    2
#define PGMID_BITS      9
#define PGTABLE_BITS    9
#define PGOFFSET_BITS   12
#define PGBIGOFFSET_BITS  (PGTABLE_BITS + PGOFFSET_BITS)

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






#define PTR2ADDR(x) ((addr_t)(uint32_t)(x))
#define ADDR2PTR(x) ((void *)(unsigned long)(x))


#ifdef PREMAP

#define KVA2PA(x) (PTR2ADDR(x))
#define PA2KVA(x) (ADDR2PTR(x))
#else
//#define KVA2PA(x) (PTR2ADDR(VMA2LMA(x)))
#endif






#define PGINDEX_P       (1ULL << 0)

#define PGMID_P         (1ULL << 0)
#define PGMID_RW        (1ULL << 1)
#define PGMID_US        (1ULL << 2)
#define PGMID_PWT       (1ULL << 3)
#define PGMID_PCD       (1ULL << 4)
#define PGMID_A         (1ULL << 5)
#define PGMID_D         (1ULL << 5)
#define PGMID_PS        (1ULL << 7)
#define PGMID_G         (1ULL << 8)
#define PGMID_XD        (1ULL << 63)





// make a pgindex_t points to pgmid_t
#define MKPGINDEX(pgmid)    (KVA2PA(pgmid) | PGINDEX_P)
#define MKPGMID_BIG(pa)     ((pa) | PGMID_P | PGMID_PS)

// walk a pgindex_t (i.e. get the VA in pgindex_t)
#define WKPGINDEX(pgindex)  (PA2KVA(PAGE_FN(pgindex)))


#endif /* !__ASSEMBLER__ */

#endif /* !_ARCH_MMU_H */

