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

#ifndef _ARCH_TRAP_H
#define _ARCH_TRAP_H

#define MODE_USR        0x10
#define MODE_FIQ        0x11
#define MODE_IRQ        0x12
#define MODE_SVC        0x13
#define MODE_MON        0x16
#define MODE_ABT        0x17
#define MODE_HYP        0x1A
#define MODE_UND        0x1B
#define MODE_SYS        0x1F

#define MODE_MASK       0x1F


#define VCT_UND 0x04
#define VCT_SVC 0x08
#define VCT_PABT 0x0C
#define VCT_DABT 0x10
#define VCT_IRQ 0x18
#define VCT_FIQ 0x1C


#ifndef __ASSEMBLER__


struct trapframe {
    // save state from the mode which caused this exception
    uint32_t r[16]; // r0-r12, SP, LR, PC
    uint32_t psr; // PSR
    uint32_t vector;
};

#endif	/* !__ASSEMBLER__ */

#endif /* _ARCH_TRAP_H */

