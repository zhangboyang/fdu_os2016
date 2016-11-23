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

// FIXME: dummy stub, just for compile, must double check !!!!!!!

#define PGINDEX_ALIGN

#define PGINDEX_BITS    2
#define PGMID_BITS      9
#define PGTABLE_BITS    9
#define PGOFFSET_BITS   12
#define PGBIGOFFSET_BITS  (PGTABLE_BITS + PGOFFSET_BITS)


typedef uint64_t pgindex_t; // lvl3
typedef uint64_t pgmid_t; // lvl2
typedef uint64_t pgtable_t; // lvl1



struct cpu {

};





enum {
    ZONE_NORMAL,
    
    MAX_MEMORY_ZONE // EOF
};


#endif /* !__ASSEMBLER__ */

#endif /* !_ARCH_MMU_H */

