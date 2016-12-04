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
#include <arch-trap.h>

.text

.globl _start
_start:
    // the kernel entry
    
    // FIXME: should we disable IRQ?
    
    // enter SYS from HYP
    // FIXME: install HYP stub?
    mrs r0, cpsr
    and r0, r0, #~MODE_MASK
    orr r0, r0, #MODE_SYS
    msr spsr_hyp, r0 // set SPSR_hyp, for eret
    ldr r0, =VMA2LMA(svc_start)
    msr elr_hyp, r0 // set ELR_hyp, for eret
    eret

svc_start:
    // jump to master_early_init
    ldr sp, =VMA2LMA(AIM_KERN_STACK_BOTTOM) // prepare stack
    b master_early_init // jump to C code
    b spin

spin: b spin





.globl master_entry
master_entry:
    // the high address entry
    ldr sp, =AIM_KERN_STACK_BOTTOM // prepare stack
    b master_init // jump to C code
    b spin


.globl get_core_id
get_core_id:
    mrc p15, 0, r0, c0, c0, 5
    and r0, #3
    bx lr

.globl slave_early_entry
slave_early_entry:
    bl get_core_id
    ldr r1, =VMA2LMA(slave_stack_lowaddr)
    ldr sp, [r1, r0, lsl #4]
    b slave_early_init

.globl slave_entry
slave_entry:
    bl get_core_id
    ldr r1, =slave_stack
    ldr sp, [r1, r0, lsl #4]
    b slave_early_init
