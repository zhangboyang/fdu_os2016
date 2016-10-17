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

.section .entry





//////////////////////////// stage 1 entry (16 bit) //////////////////////////

// note: functions in 16-bit mode using pascal calling convention

.globl _start
_start:
bootasm:
.code16

// prepare environment
  cli
  cld
  xorw    %ax,%ax
  movw    %ax,%ds
  movw    %ax,%es
  movw    %ax,%ss
  ljmp $0x0000, $1f
1:
  movw    $bootasm, %sp

// display hello message
  push $hellostr
  call bootprint
  
  call bootpanic







// function: print message using BIOS functions
//   DS:[SP+2H] = c-style string to print
bootprint:
  push %bp
  push %si
  mov %sp, %bp
  mov 6(%bp), %si
1:
  lodsb
  test %al, %al
  jnz 2f
  pop %si
  pop %bp
  ret $0x2
2:
  mov $0x0e, %ah
  int $0x10
  jmp 1b

// function: display error message and spin forever
bootpanic:
  push $errorstr
  call bootprint
  pop %dx // DX will be address of caller's next instruction
  mov %sp, %bp
  dec %bp
  movb $0, (%bp)
  sub $8, %sp
  mov $4, %cx
2:
  mov %dx, %ax
  shr $4, %dx
  and $0xf, %al
  cmp $0x9, %al
  jbe 3f
  add $0x10, %al
3:
  add $0x30, %al
  dec %bp
  mov %al, (%bp)
  loop 2b
  push %bp
  call bootprint
1:
  jmp 1b

hellostr:
.asciz "stage0 ... "
errorstr:
.asciz "error! ip="





.section .entry_end
.fill 0x10, 1, 0x90






//////////////////////////// stage 1 entry (16 bit) //////////////////////////

.text

.code16
/*
   ZBY: most code are copied from xv6
*/

// the real-mode bootloader
bootloader_start:
  cli                         # BIOS enabled interrupts; disable
  cld

  # Zero data segment registers DS, ES, and SS.
  xorw    %ax,%ax             # Set %ax to zero
  movw    %ax,%ds             # -> Data Segment
  movw    %ax,%es             # -> Extra Segment
  movw    %ax,%ss             # -> Stack Segment

  # Physical address line A20 is tied to zero so that the first PCs 
  # with 2 MB would run software that assumed 1 MB.  Undo that.
seta20.1:
  inb     $0x64,%al               # Wait for not busy
  testb   $0x2,%al
  jnz     seta20.1

  movb    $0xd1,%al               # 0xd1 -> port 0x64
  outb    %al,$0x64

seta20.2:
  inb     $0x64,%al               # Wait for not busy
  testb   $0x2,%al
  jnz     seta20.2

  movb    $0xdf,%al               # 0xdf -> port 0x60
  outb    %al,$0x60

  # Switch from real to protected mode.  Use a bootstrap GDT that makes
  # virtual addresses map directly to physical addresses so that the
  # effective memory map doesn't change during the transition.
  lgdt    gdtdesc
  movl    %cr0, %eax
  orl     $CR0_PE, %eax
  movl    %eax, %cr0

//PAGEBREAK!
  # Complete the transition to 32-bit protected mode by using a long jmp
  # to reload %cs and %eip.  The segment descriptors are set up with no
  # translation, so that the mapping is still the identity mapping.
  ljmp    $(SEG_KCODE<<3), $start32

.code32  # Tell assembler to generate 32-bit code now.
start32:
  # Set up the protected-mode data segment registers
  movw    $(SEG_KDATA<<3), %ax    # Our data segment selector
  movw    %ax, %ds                # -> DS: Data Segment
  movw    %ax, %es                # -> ES: Extra Segment
  movw    %ax, %ss                # -> SS: Stack Segment
  movw    $0, %ax                 # Zero segments not ready for use
  movw    %ax, %fs                # -> FS
  movw    %ax, %gs                # -> GS

  # Set up the stack pointer and call into C.
  movl    $_start, %esp
  call    stage0

spin: // infloop
  jmp     spin



.data

# Bootstrap GDT
.p2align 2                                # force 4 byte alignment
gdt:
  SEG_NULLASM                             # null seg
  SEG_ASM(STA_X|STA_R, 0x0, 0xffffffff)   # code seg
  SEG_ASM(STA_W, 0x0, 0xffffffff)         # data seg

gdtdesc:
  .word   (gdtdesc - gdt - 1)             # sizeof(gdt) - 1
  .long   gdt                             # address gdt



