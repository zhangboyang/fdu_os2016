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

#include <asm-generic/vmaim.lds.h>
#include <aim/mmu.h>

/*
 * Using the C preprocessor, we allow includes and macro expansion in this
 * linker script.
 */

#define i386_align 4096

ENTRY(_start)

PHDRS
{
    text PT_LOAD FLAGS(5);
    init PT_LOAD FLAGS(4);
    rodata PT_LOAD FLAGS(4);
    data PT_LOAD FLAGS(6);
}


SECTIONS
{
    . = KERN_START + KOFFSET;
    KERN_START_LOW = . - KOFFSET;
    KERN_START_HIGH = .;
    
    .text : AT(KERN_START) { *(.text*); } : text
    
    . = ALIGN(i386_align);
    .init : { EARLY_INIT(i386_align) NORM_INIT(i386_align) LATE_INIT(i386_align) } : init
    
    . = ALIGN(i386_align);
    .rodata : { *(.rodata*); } : rodata

    . = ALIGN(i386_align);    
    .data : { *(.data); } : data
    

    . = ALIGN(i386_align);
    AIM_KERN_BSS_START = .;
    .bss : { *(.bss); } : data
    AIM_KERN_BSS_END = .;

    . = ALIGN(i386_align);
    AIM_KERN_STACK_START = .;
    .stack : { . += KSTACKSIZE; } : data
    AIM_KERN_STACK_BOTTOM = .;
    
    . = ALIGN(i386_align);
    KERN_END_LOW = . - KOFFSET;
    KERN_END_HIGH = .;
    
    /DISCARD/ : {
        *(.eh_frame);
    }
}
