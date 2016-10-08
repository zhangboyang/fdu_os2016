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

#define __LDSCRIPT__

/*
 * Using the C preprocessor, we allow includes and macro expansion in this
 * linker script.
 */

ENTRY(_start)

SECTIONS
{
    . = KERN_START;
    .text : { *(.text) }
    .data : { *(.data) }
    
    AIM_KERN_BSS_START = .;
    .bss : { *(.bss) }
    AIM_KERN_BSS_END = .;
    
	 . = ALIGN(4096);
    AIM_KERN_STACK_LOWLIMIT = .;
    . += KSTACKSIZE;
    AIM_KERN_STACK_HIGHLIMIT = .;
}
