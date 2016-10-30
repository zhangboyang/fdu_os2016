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

#define KOFFSET KERN_BASE
#define KTOP 0xFE000000

#define VMA2LMA(x) ((x) - KOFFSET)


/* addresses before and after early MMU mapping */
#define __premap_addr(kva)	(ULCAST(kva) - KERN_BASE)
#define __postmap_addr(pa)	(ULCAST(pa) + KERN_BASE)

#define LOWADDR(highaddr) ((typeof((highaddr) + 0)) ((ULCAST(highaddr)) - KERN_BASE))

/* kernel virtual address and physical address conversion */
#define kva2pa(kva)		(ULCAST(kva) - KERN_BASE)
#define pa2kva(pa)		(PTRCAST(pa) + KERN_BASE)


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



// Eflags register
#define FL_CF           0x00000001      // Carry Flag
#define FL_PF           0x00000004      // Parity Flag
#define FL_AF           0x00000010      // Auxiliary carry Flag
#define FL_ZF           0x00000040      // Zero Flag
#define FL_SF           0x00000080      // Sign Flag
#define FL_TF           0x00000100      // Trap Flag
#define FL_IF           0x00000200      // Interrupt Enable
#define FL_DF           0x00000400      // Direction Flag
#define FL_OF           0x00000800      // Overflow Flag
#define FL_IOPL_MASK    0x00003000      // I/O Privilege Level bitmask
#define FL_IOPL_0       0x00000000      //   IOPL == 0
#define FL_IOPL_1       0x00001000      //   IOPL == 1
#define FL_IOPL_2       0x00002000      //   IOPL == 2
#define FL_IOPL_3       0x00003000      //   IOPL == 3
#define FL_NT           0x00004000      // Nested Task
#define FL_RF           0x00010000      // Resume Flag
#define FL_VM           0x00020000      // Virtual 8086 mode
#define FL_AC           0x00040000      // Alignment Check
#define FL_VIF          0x00080000      // Virtual Interrupt Flag
#define FL_VIP          0x00100000      // Virtual Interrupt Pending
#define FL_ID           0x00200000      // ID flag


#define SEG_KCODE 1  // kernel code
#define SEG_KDATA 2  // kernel data+stack
#define SEG_KCPU  3  // kernel per-cpu data
#define SEG_UCODE 4  // user code
#define SEG_UDATA 5  // user data+stack
#define SEG_TSS   6  // this process's task state





#ifndef __ASSEMBLER__




// Task state segment format
struct taskstate {
  uint32_t link;         // Old ts selector
  uint32_t esp0;         // Stack pointers and segment selectors
  uint16_t ss0;        //   after an increase in privilege level
  uint16_t padding1;
  uint32_t *esp1;
  uint16_t ss1;
  uint16_t padding2;
  uint32_t *esp2;
  uint16_t ss2;
  uint16_t padding3;
  void *cr3;         // Page directory base
  uint32_t *eip;         // Saved state from last task switch
  uint32_t eflags;
  uint32_t eax;          // More saved state (registers)
  uint32_t ecx;
  uint32_t edx;
  uint32_t ebx;
  uint32_t *esp;
  uint32_t *ebp;
  uint32_t esi;
  uint32_t edi;
  uint16_t es;         // Even more saved state (segment selectors)
  uint16_t padding4;
  uint16_t cs;
  uint16_t padding5;
  uint16_t ss;
  uint16_t padding6;
  uint16_t ds;
  uint16_t padding7;
  uint16_t fs;
  uint16_t padding8;
  uint16_t gs;
  uint16_t padding9;
  uint16_t ldt;
  uint16_t padding10;
  uint16_t t;          // Trap on task switch
  uint16_t iomb;       // I/O map base address
};

// PAGEBREAK: 12
// Gate descriptors for interrupts and traps
struct gatedesc {
  uint32_t off_15_0 : 16;   // low 16 bits of offset in segment
  uint32_t cs : 16;         // code segment selector
  uint32_t args : 5;        // # args, 0 for interrupt/trap gates
  uint32_t rsv1 : 3;        // reserved(should be zero I guess)
  uint32_t type : 4;        // type(STS_{TG,IG32,TG32})
  uint32_t s : 1;           // must be 0 (system)
  uint32_t dpl : 2;         // descriptor(meaning new) privilege level
  uint32_t p : 1;           // Present
  uint32_t off_31_16 : 16;  // high bits of offset in segment
};

// Set up a normal interrupt/trap gate descriptor.
// - istrap: 1 for a trap (= exception) gate, 0 for an interrupt gate.
//   interrupt gate clears FL_IF, trap gate leaves FL_IF alone
// - sel: Code segment selector for interrupt/trap handler
// - off: Offset in code segment for interrupt/trap handler
// - dpl: Descriptor Privilege Level -
//        the privilege level required for software to invoke
//        this interrupt/trap gate explicitly using an int instruction.
#define SETGATE(gate, istrap, sel, off, d)                \
{                                                         \
  (gate).off_15_0 = (uint32_t)(off) & 0xffff;                \
  (gate).cs = (sel);                                      \
  (gate).args = 0;                                        \
  (gate).rsv1 = 0;                                        \
  (gate).type = (istrap) ? STS_TG32 : STS_IG32;           \
  (gate).s = 0;                                           \
  (gate).dpl = (d);                                       \
  (gate).p = 1;                                           \
  (gate).off_31_16 = (uint32_t)(off) >> 16;                  \
}



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



#define MAX_MACHINE_MEMORY_MAP 20
struct machine_memory_map {
    uint64_t base, size;
    uint32_t type, unknown;
};

extern struct machine_memory_map mach_mem_map[MAX_MACHINE_MEMORY_MAP];
extern unsigned nr_mach_mem_map;


extern uint8_t KERN_START_HIGH[];
extern uint8_t KERN_END_HIGH[];
extern uint8_t KERN_START_LOW[];
extern uint8_t KERN_END_LOW[];
extern size_t size_after_kernel; // how many bytes can we use after the kernel


enum {
    ZONE_DMA,
    ZONE_NORMAL,
    ZONE_HIGHMEM,
    
    MAX_MEMORY_ZONE // EOF
};


struct cpu {
    
};

#endif /* !__ASSEMBLER__ */

#endif /* !_ARCH_MMU_H */

