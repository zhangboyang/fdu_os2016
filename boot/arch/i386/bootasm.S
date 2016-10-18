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



//////////////////////////// stage 1 entry (16 bit) //////////////////////////

// note: functions in 16-bit mode using pascal calling convention
.section .entry, "ax"
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

// check INT 13H extensions
  mov $0x41, %ah
  mov $0x80, %dl // first HDD
  mov $0x55AA, %bx
  int $0x13
  jnc 1f // check BIOS error
  call bootpanic
1:
  cmp $0xAA55, %bx // check magic
  je 1f
  call bootpanic
1:
  test $0x1, %cx // check extensions bits
  jnz 1f
  call bootpanic
1:

  
// read ELF header
  call getpartition
  push $0x1000
  push $8     // read 4096 bytes, 4096/512=8
  push %dx
  push %ax
  call bootreaddisk

// check ELF magic
  cmpw $0x457f, (0x1000)
  jne 1f
  cmpw $0x464c, (0x1002)
  je 2f
1:
  call bootpanic
2:


// load ELF program headers
  mov (0x101c), %bx
  add $0x1000, %bx
  mov (0x102c), %si
1:
  push %bx
  call load_elf_phdr
  add $32, %bx
  dec %si
  jnz 1b
  
  
// jump to 'high' address
  push $jumpstr
  call bootprint
  jmp bootloader_start







// function: load PHDR from ELF file
load_elf_phdr:
  push %bp
  mov %sp, %bp
  push %bx
  push %si
  push %di
  mov 4(%bp), %bx

  mov 0xc(%bx), %dx // paddr
  
  // overwrite myself is not allowed
  // since it might overwrite partition LBA
  // which we will use later
  cmp $0x7c00, %dx
  je 2f

  push %dx // push buffer
  push %dx
  call bootprintword
  push $phdr_str1
  call bootprint

  mov 0x10(%bx), %ax // AX: (short) filesz
  test $0x1ff, %ax
  jz 1f
  add $0x200, %ax
1:
  shr $9, %ax
  push %ax // push sector count
  push %ax
  call bootprintword
  push $phdr_str2
  call bootprint
  
  mov 0x4(%bx), %si
  mov 0x6(%bx), %di // DI:SI  offset
  shrd $9, %di, %si
  shr $9, %di
  call getpartition // DX:AX  LBA
  add %ax, %si
  adc %dx, %di // DI:SI  LBA+offset
  push %di
  push %si // push LBA
  push %di
  call bootprintword
  push %si
  call bootprintword
  push $phdr_str3
  call bootprint

  call bootreaddisk // read disk to load this phdr
  
  // cleanup [filesz, memsz)
  mov 0x14(%bx), %cx // memsz
  mov 0x10(%bx), %dx // filesz
  mov 0xc(%bx), %di // paddr
  add %dx, %di // DI: paddr + filesz
  sub %dx, %cx // CX: memsz - filesz
  xor %ax, %ax
  rep stosb
  
2:
  pop %di
  pop %si
  pop %bx
  leave
  ret


// function: get LBA of first partition, to DX:AX
getpartition:
  /*push $phdr_str3
  call bootprint
  mov (0x7C00 + 0x1BE + 0x10 * 0 + 0x8), %ax // LOW
  push %ax
  call bootprintword
  mov (0x7C00 + 0x1BE + 0x10 * 0 + 0x8 + 0x2), %dx // HIGH
  push %dx
  call bootprintword*/
  mov (0x7C00 + 0x1BE + 0x10 * 0 + 0x8), %ax // LOW
  mov (0x7C00 + 0x1BE + 0x10 * 0 + 0x8 + 0x2), %dx // HIGH
  ret

// function: read disk sector using BIOS functions
//   usage: push buffer; push COUNT; push HIGH; push LOW; call bootreaddisk;
bootreaddisk:
  push %si
  push %bp
  mov %sp, %bp
  and $0xfff0, %sp
  pushw $0
  pushw $0
  pushw 8(%bp) // HIGH
  pushw 6(%bp) // LOW
  pushw $0  // segment
  pushw 12(%bp) // buffer
  pushw 10(%bp) // COUNT
  pushw $1
  mov $0x42, %ah
  mov $0x80, %dl // first HDD
  mov %sp, %si
  int $0x13
  jnc 1f
  call bootpanic
1:
  leave
  pop %si
  ret $0x8



// function: print message using BIOS functions
//   DS:ARG1 = c-style string to print
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

// function: print a word in dex using BIOS functions
//   ARG1 = word to print
bootprintword:
  push %bp
  pushw $0x0
  mov %sp, %bp
  mov 6(%bp), %dx
  sub $4, %sp
  mov $4, %cx
2:
  mov %dx, %ax
  shr $4, %dx
  and $0xf, %al
  cmp $0x9, %al
  jbe 3f
  add $0x7, %al
3:
  add $0x30, %al
  dec %bp
  mov %al, (%bp)
  loop 2b
  push %bp
  call bootprint
  add $6, %sp
  pop %bp
  ret $2

// function: display error message and spin forever
bootpanic:
  push $errorstr
  call bootprint
  call bootprintword
1:
  jmp 1b

hellostr:
.asciz "boot stage0 ...\r\n"
errorstr:
.asciz "error! [sp]="
jumpstr:
.asciz "jump to stage1 ...\r\n"

// phdr msg format: [PADDR] <= [SECTORS], [LBA]
phdr_str1:
.asciz " <= "
phdr_str2:
.asciz " SECTS, LBA "
phdr_str3:
.asciz "\r\n"








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
  jmp retn_to_realmode
  call    bootmain

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






//////////// return from 32-bit protected mode to real mode //////////////////

// function to read disk real mode
//    void readsect_realmode(unsigned lowaddr, unsigned int sect)
//    return 1 if read OK, 0 on read fail
// most code to retn to real mode is copied from linux 2.4.0
.text
.code32  
.globl readsect_realmode
readsect_realmode:
  pusha // save all registers
  mov 4(%esp), %esi // LBA
  mov 8(%esp), %edi // buffer
  
  // jmp to 16-bit protected mode
  lgdt gdtdesc_16
  ljmp $(1<<3), $1f
.code16
1:
  movl    $(2<<3), %eax
  movl    %eax, %ds
  movl    %eax, %es
  movl    %eax, %ss
  movl    $0, %eax
  movl    %eax, %fs
  movl    %eax, %gs

  // disable protected mode
  movl    %cr0, %eax
  orl     $CR0_PE, %eax
  movl    %eax, %cr0

  // ljmp to real mode
  ljmp $0x0000, $1f
  xorw    %ax,%ax
  movw    %ax,%ds
  movw    %ax,%es
  movw    %ax,%ss

  // read disk
  mov %sp, %bp
  and $0xfff0, %sp
  pushw $0
  pushw $0
  pushl %esi
  pushw $0  // segment
  pushw %di
  pushw $1
  pushw $1
  mov $0x42, %ah
  mov $0x80, %dl // first HDD
  mov %sp, %si
  int $0x13
  mov $0, %al
  jc 1b
  inc %al
1:

  // go back to protected-mode
  lgdt    gdtdesc
  movl    %cr0, %eax
  orl     $CR0_PE, %eax
  movl    %eax, %cr0

  ljmp    $(SEG_KCODE<<3), $start32

.code32
start32:
  movw    $(SEG_KDATA<<3), %ax
  movw    %ax, %ds
  movw    %ax, %es
  movw    %ax, %ss
  movw    $0, %ax
  movw    %ax, %fs
  movw    %ax, %gs
  movzbl %al, %eax
  mov %eax, 0x1c(%esp)
  popa
  retn



gdt_16:
.quad 0x0000000000000000 /* Null descriptor */
.quad 0x00009a000000ffff /* 16-bit real-mode 64k code at 0x00000000 */
.quad 0x000092000100ffff /* 16-bit real-mode 64k data at 0x00000100 */
gdtdesc_16:
  .word   (gdtdesc_16 - gdt_16 - 1)
  .long   gdt_16

