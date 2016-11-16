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

#include <arch-boot.h>

// bootloader code for raspi2 kernel7.img
// some code copied from :
//    https://github.com/dwelch67/raspberrypi.git
//      boards/pi2/SVC_BOOT/uart01

.section .entry
.globl _start
_start:
    b bootasm // jmp to bootasm

.text
spin: b spin // infloop
bootasm: // asm entry
    mrc p15,0,r0,c0,c0,5 ;@ MPIDR
    mov r1,#0xFF
    ands r1,r1,r0
    bne spin
    ldr sp, =ELF_INFO_START // prepare stack
    b bootmain // jmp to C code
    b spin

