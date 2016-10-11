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
#include <aim/mmu.h>


/*
   ZBY: use PAE for i386
*/



static __attribute((aligned(PGINDEX_ALIGN))) pgindex_t __boot_page_index[PGINDEX_SIZE];

static __attribute((aligned(PGMID_ALIGN))) pgmid_t __boot_page_mid_all[PGINDEX_SIZE][PGMID_SIZE];


/*
    ZBY: instead clearing the @boot_page_index
         we initialize the @boot_page_index using kernel .bss memory
         only init first two level of pgindex
         we will use 2MB pages later
*/
void page_index_clear(pgindex_t *boot_page_index)
{
    int i, j;
    
    // init the first level of pgindex
    for (i = 0; i < PGINDEX_SIZE; i++) {
        __boot_page_index[i] = MKPGINDEX(__boot_page_mid_all[i]);
    }
    
    // clear the second level of pgindex
    memset(__boot_page_mid_all, 0, sizeof(__boot_page_mid_all));
}


void mmu_init(pgindex_t *boot_page_index)
{
    page_index_init(boot_page_index);
}



void early_mm_init(void)
{
    // FIXME: add some early mapping

    mmu_init(__boot_page_index);
}

