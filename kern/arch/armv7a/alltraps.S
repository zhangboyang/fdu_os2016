/*
    this file is added by ZBY
    construct a trap frame, then call the C routine
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <aim/mmu.h>
#include <arch-trap.h>

// exception vectors
.text
.balign 32
.global vectors
vectors:
b exception_00
b exception_04
b exception_08
b exception_0C
b exception_10
b exception_14
b exception_18
b exception_1C

.text
spin: b spin
exception_00: b spin // not used
exception_04: // undefined instruction
  sub lr, lr, #4
  b enter_svc
exception_08: // supervisor call
  b enter_svc
exception_0C: // prefetch abort
  sub lr, lr, #4
  b enter_svc
exception_10: // data abort
  sub lr, lr, #8
  b enter_svc
exception_14: b spin // not used
exception_18: // IRQ interrupt
  sub lr, lr, #4
  b enter_svc
exception_1C: // FIQ interrupt
  sub lr, lr, #4
  b enter_svc

enter_svc:
  // save [r0, lr, spsr]
  //       ^SP +4  +8
  stmia sp, {r0, lr}
  mrs r0, spsr
  str r0, [sp, #8]
  // prepare spsr
  mrs r0, cpsr
  and r0, r0, #~MODE_MASK
  orr r0, r0, #MODE_SVC
  msr spsr_cxsf, r0
  // r0 is pointer to saved registers
  mov r0, sp
  // do switch
  ldr lr, =alltraps
  movs pc, lr // branch and switch mode

alltraps:
  ldr lr, [r0, #8] // lr = saved spsr (i.e. user PSR)
  push {lr}
  ldr lr, [r0, #4] // lr = saved lr (i.e. user PC)
  push {lr}
  mrs lr, lr_usr
  push {lr}
  mrs lr, sp_usr
  push {lr}
  
  ldr r0, [r0] // r0 = saved r0
  push {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12}
  
  mov r0, sp // arg1 = pointer to trapframe
  ldr lr, =trap
  blx lr
  b spin

