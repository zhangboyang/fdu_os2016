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
#include <platform.h>
#include <aim/init.h>

#define KVA2PA(x) (PTR2ADDR(x))
#define PA2KVA(x) (ADDR2PTR(x))


static __attribute((aligned(PGINDEX_ALIGN))) pgindex_t __boot_page_index[PGINDEX_SIZE];
static __attribute((aligned(PGMID_ALIGN))) pgmid_t __boot_page_mid_all[PGINDEX_SIZE][PGMID_SIZE];


bool early_mapping_valid(struct early_mapping *entry)
{
/*dump_memory(entry, sizeof(*entry));
kprintf("result=%d\n", BIGPAGE_OFF(entry->paddr) == 0 &&
           BIGPAGE_OFF(PTR2ADDR(entry->vaddr)) == 0 &&
           BIGPAGE_OFF(entry->size) == 0);*/

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
//__boot_page_index[i] = 0b11000000001; kprintf("&__boot_page_index[i]=%p\n", &__boot_page_index[i]);
//        kprintf("i=%d tbl=%p val=%llx\n", i, __boot_page_mid_all[i], __boot_page_index[i]);
    }
    
    // clear the second level of pgindex (clear P bit to 0)
    memset(__boot_page_mid_all, 0, sizeof(__boot_page_mid_all));
}

int page_index_early_map(pgindex_t *boot_page_index, addr_t paddr, void *vaddr, size_t size, int type)
{
    // assert for alignment
    assert(BIGPAGE_OFF(paddr) == 0);
    assert(BIGPAGE_OFF(PTR2ADDR(vaddr)) == 0);
    assert(BIGPAGE_OFF(size) == 0);
    
    uint32_t SH, AttrIndx;
    switch (type) {
        case EARLY_MAPPING_MEMORY: SH = SH_INNER, AttrIndx = MAIR_NORMAL_IDX; break;
        case EARLY_MAPPING_DEVICE: SH = SH_OUTER, AttrIndx = MAIR_DEVICE_IDX; break;
        case EARLY_MAPPING_FRAMEBUFFER: SH = SH_OUTER, AttrIndx = MAIR_FRAMEBUFFER_IDX; break;
        default: panic("unknown mapping type %d", type);
    }
    
    kprintf("early mapping: paddr=%016llx vaddr=%08p size=%08lx type=%d\n", paddr, vaddr, size, type);
    addr_t pa, va;
    for (pa = paddr, va = PTR2ADDR(vaddr); pa < paddr + size; pa += BIGPAGE_SIZE, va += BIGPAGE_SIZE) {
        // map VA to PA
        pgmid_t *pgmid = WKPGINDEX(boot_page_index[PGINDEX_FN(va)]);
        pgmid[PGMID_FN(va)] = MKPGMID_BIG(pa, SH, AttrIndx);
        //kprintf("pgmid=%llx midfn=%llx val=%llx\n", ULLCAST(ULCAST(pgmid)), ULLCAST(PGMID_FN(va)), ULLCAST(pgmid[PGMID_FN(va)]));
    }
    
    // FIXME: device memory attr
    
    return 0;
}

void early_mm_init(void)
{
    // initialize high address
    mach_add_mapping();
    mmu_init(__boot_page_index);
}

void mmu_jump()
{
    kprintf("prepare to perform mmu jump!\n");
    
    union {
        uint32_t val;
        struct {
            uint32_t T0SZ : 3;
            uint32_t : 4;
            uint32_t EPD0 : 1;
            uint32_t IRGN0 : 2;
            uint32_t ORGN0 : 2;
            uint32_t SH0 : 2;
            uint32_t : 2;
            uint32_t T1SZ : 3;
            uint32_t : 3;
            uint32_t A1 : 1;
            uint32_t EPD1 : 1;
            uint32_t IRGN1 : 2;
            uint32_t ORGN1 : 2;
            uint32_t SH1 : 2;
            uint32_t : 1;
            uint32_t EAE : 1;
        };
    } TTBCR;
    //__asm__ __volatile__ ("mrc p15, 0, %0, c2, c0, 2":"=r"(TTBCR.val));
    //kprintf("old TTBCR = %08x\n", TTBCR.val);
    TTBCR.val = 0; // reset all fields to zero
    TTBCR.EAE = 1;
    TTBCR.ORGN0 = 0b01; // WBWA
    TTBCR.IRGN0 = 0b01; // WBWA
    TTBCR.SH0 = SH_NONE;
//    kprintf("new TTBCR = %08x\n", TTBCR.val);
    __asm__ __volatile__ ("mcr p15, 0, %0, c2, c0, 2"::"r"(TTBCR.val));
//    TTBCR.val = -1; __asm__ __volatile__ ("mrc p15, 0, %0, c2, c0, 2":"=r"(TTBCR.val)); kprintf("read TTBCR = %08x\n", TTBCR.val);
    
    union {
        uint64_t addr;
        struct {
            uint32_t low;
            uint32_t high;
        };
        struct {
            uint64_t : 48;
            uint64_t ASID : 8;
        };
    } TTBR0;
    TTBR0.addr = ULCAST(__boot_page_index);
//    kprintf("new TTBR0 = %llx\n", TTBR0.addr);
    __asm__ __volatile__ ("mcrr p15, 0, %0, %1, c2"::"r"(TTBR0.low), "r"(TTBR0.high));
//    TTBR0.addr = -1; __asm__ __volatile__ ("mrrc p15, 0, %0, %1, c2":"=r"(TTBR0.low), "=r"(TTBR0.high)); kprintf("read TTBR0 = %08x %08x\n", TTBR0.high, TTBR0.low);
    
    uint32_t MAIR0 = MKMAIR(MAIR_NORMAL, MAIR_DEVICE, MAIR_FRAMEBUFFER, 0);
    __asm__ __volatile__ ("mcr p15, 0, %0, c10, c2, 0"::"r"(MAIR0));
//    MAIR0 = -1; __asm__ __volatile__ ("mrc p15, 0, %0, c10, c2, 0":"=r"(MAIR0)); kprintf("read MAIR0 = %x\n", MAIR0);
fbcls(LOWADDR(&fb), 0xff);

    union {
        uint32_t val;
        struct {
            uint32_t M : 1;
            uint32_t A : 1;
            uint32_t C : 1;
            uint32_t : 2;
            uint32_t CB15BEN : 1;
            uint32_t : 1;
            uint32_t B : 1;
            uint32_t : 2;
            uint32_t SW : 1;
            uint32_t Z : 1;
            uint32_t I : 1;
            uint32_t V : 1;
            uint32_t RR : 1;
            uint32_t : 2;
            uint32_t HA : 1;
            uint32_t : 1;
            uint32_t WXN : 1;
            uint32_t UWXN : 1;
            uint32_t FI : 1;
            uint32_t U : 1;
            uint32_t : 1;
            uint32_t VE : 1;
            uint32_t EE : 1;
            uint32_t : 1;
            uint32_t NMFI : 1;
            uint32_t TRE : 1;
            uint32_t AFE : 1;
            uint32_t TE : 1;
            uint32_t : 1;
        };
    } SCTLR;
    __asm__ __volatile__ ("mrc p15, 0, %0, c1, c0, 0":"=r"(SCTLR.val));
//    kprintf("old SCTLR = %08x\n", SCTLR.val);
    SCTLR.M = 1;
    SCTLR.AFE = 1;
    SCTLR.TRE = 1;
    SCTLR.I = 1;
    SCTLR.Z = 1;
    SCTLR.C = 1;
    
    kprintf("new SCTLR = %08x\n", SCTLR.val);
    __asm__ __volatile__ ("mcr p15, 0, %0, c1, c0, 0"::"r"(SCTLR.val));

fbcls(LOWADDR(&fb), 0xff);

    abs_jump(master_entry);
}


__asm__ (
".globl abs_jump\n"
"abs_jump:\n"
"   b r0\n"
);
#undef KVA2PA
#undef PA2KVA
