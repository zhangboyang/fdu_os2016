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
    // the kernel entry
    
    // FIXME: should we disable IRQ?
    
    // enter SVC from HYP
    mrs r0, cpsr
    and r0, r0, #~0x1F
    orr r0, r0, #0x13 // SVC
    msr cpsr_c, r0
    
    // jump to master_early_init
    ldr sp, =VMA2LMA(AIM_KERN_STACK_BOTTOM) // prepare stack
    b master_early_init // jump to C code
    b spin

spin: b spin

