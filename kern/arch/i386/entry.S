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

#include <aim/mmu.h>


.text

.globl _start
_start:

.globl master_early_entry
master_early_entry:

    // disable interrupts
    cli
    cld
    
    // reload GDT using kernel data segment
    lgdt VMA2LMA(gdtdesc)
    ljmp    $(SEG_KCODE<<3), $start32
start32:
    movw    $(SEG_KDATA<<3), %ax    # Our data segment selector
    movw    %ax, %ds                # -> DS: Data Segment
    movw    %ax, %es                # -> ES: Extra Segment
    movw    %ax, %ss                # -> SS: Stack Segment
    movw    $0, %ax                 # Zero segments not ready for use
    movw    %ax, %fs                # -> FS
    movw    %ax, %gs                # -> GS
    
    // clean up bss
    xor %eax, %eax
    mov $VMA2LMA(AIM_KERN_BSS_START), %edi
    mov $VMA2LMA(AIM_KERN_BSS_END), %ecx
    sub %edi, %ecx
    rep stosb
    
    // prepare stack for C code
    mov $VMA2LMA(AIM_KERN_STACK_BOTTOM), %esp
    xor %ebp, %ebp // make stack backtrace happy
    
    // call master_early_init()
    call master_early_init
    
    // infloop in case of master_early_init() returns
spin:
    jmp spin





.rodata
.p2align 2                                # force 4 byte alignment
gdt:
  SEG_NULLASM                             # null seg
  SEG_ASM(STA_X|STA_R, 0x0, 0xffffffff)   # code seg
  SEG_ASM(STA_W, 0x0, 0xffffffff)         # data seg

gdtdesc:
  .word   (gdtdesc - gdt - 1)             # sizeof(gdt) - 1
  .long   gdt                             # address gdt
