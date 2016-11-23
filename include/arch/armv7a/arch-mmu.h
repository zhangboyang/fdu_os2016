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
#define SH_OUTER 0b11ULL
#define SH_INNER 0b11ULL

// FIXME: init MAIR
#define MAIR_NORMAL 0

// make stage 1 lower attribute, in place
#define MKS1LA(nG, AF, SH, AP, NS, AttrIndx) ( \
    ((nG) << 11ULL) | \
    ((AF) << 10ULL) | \
    ((SH) << 8ULL) | \
    ((AP) << 6ULL) | \
    ((NS) << 5ULL) | \
    ((AttrIndx) << 2ULL))
// make stage 1 upper attribute, in place
#define MKS1UA(SoftUse, XN, PXN, ContHint) ( \
    ((SoftUse) << 55ULL) | \
    ((XN) << 54ULL) | \
    ((PXN) << 53ULL) | \
    ((ContHint) << 52ULL))

// make a pgindex_t points to pgmid_t
#define MKPGINDEX(pgmid)    (KVA2PA(pgmid) | PGTABLEENTRY_P)
#define MKPGMID_BIG(pa)     ((pa) | PGBLOCKENTRY_P | MKS1UA(0, 0, 0, 0) | MKS1LA(0, 0, SH_INNER, AP_SYSRW, 1, MAIR_NORMAL))





struct cpu {

};




// only one memory zone
enum {
    ZONE_NORMAL,
    MAX_MEMORY_ZONE // EOF
};


#endif /* !__ASSEMBLER__ */

#endif /* !_ARCH_MMU_H */

