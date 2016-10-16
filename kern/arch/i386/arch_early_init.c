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
#include <aim/init.h>
#include <aim/early_kmmap.h>
#include <arch-mmu.h>

void arch_early_init(void)
{
    early_mapping_clear();
    
    addr_t ksize = KERN_END_HIGH - KOFFSET;
    struct early_mapping entry = {
		.paddr	= 0,
		.vaddr	= KOFFSET,
		.size	= ROUNDUP(ksize, BIGPAGE_SIZE),
		.type	= EARLY_MAPPING_MEMORY
	};
	
	early_mapping_add(&entry);
}

