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

#ifndef __ASSEMBLER__

typedef uint64_t pgindex_t; // lvl3
typedef uint64_t pgmid_t; // lvl2
typedef uint64_t pgtable_t; // lvl1


#define PGINDEX_BITS    2ULL
#define PGMID_BITS      9ULL
#define PGTABLE_BITS    9ULL
#define PGOFFSET_BITS   12ULL
#define PGBIGOFFSET_BITS  (PGTABLE_BITS + PGOFFSET_BITS)


#define PGTABLEENTRY_P 0b11ULL
#define PGBLOCKENTRY_P 0b01ULL
#define PGPAGEENTRY_P 0b11ULL

#define AP_SYSRW 0b00ULL
#define AP_USRRW 0b01ULL
#define AP_SYSRO 0b10ULL
#define AP_USRRO 0b11ULL

#define SH_NONE  0b00ULL
#define SH_OUTER 0b10ULL
#define SH_INNER 0b11ULL

// FIXME: init MAIR
#define MAIR_NORMAL_IDX 0b000
#define MAIR_DEVICE_IDX 0b001
#define MAIR_FRAMEBUFFER 0b010

#define MAIR_NORMAL 0b11111111
#define MAIR_DEVICE 0b00000000
#define MAIR_FRAMEBUFFER 0b00000000


#define MKMAIR(attr0, attr1, attr2, attr3) ( \
    ((attr0) << 0) | \
    ((attr1) << 8) | \
    ((attr2) << 16) | \
    ((attr3) << 24))

#define ULLCAST(x) ((unsigned long long)(x))
// make stage 1 lower attribute, in place
#define MKS1LA(nG, AF, SH, AP, NS, AttrIndx) ( \
    (ULLCAST(nG) << 11) | \
    (ULLCAST(AF) << 10) | \
    (ULLCAST(SH) << 8) | \
    (ULLCAST(AP) << 6) | \
    (ULLCAST(NS) << 5) | \
    (ULLCAST(AttrIndx) << 2))
// make stage 1 upper attribute, in place
#define MKS1UA(SoftUse, XN, PXN, ContHint) ( \
    (ULLCAST(SoftUse) << 55) | \
    (ULLCAST(XN) << 54) | \
    (ULLCAST(PXN) << 53) | \
    (ULLCAST(ContHint) << 52))

// make a pgindex_t points to pgmid_t
#define MKPGINDEX(pgmid)    (KVA2PA(pgmid) | PGTABLEENTRY_P)
#define MKPGMID_BIG(pa)     ((pa) | PGBLOCKENTRY_P | MKS1UA(0, 0, 0, 0) | MKS1LA(0, 1, SH_INNER, AP_SYSRW, 0, MAIR_NORMAL_IDX))





struct cpu {

};




// only one memory zone
enum {
    ZONE_NORMAL,
    MAX_MEMORY_ZONE // EOF
};


#endif /* !__ASSEMBLER__ */

#endif /* !_ARCH_MMU_H */

