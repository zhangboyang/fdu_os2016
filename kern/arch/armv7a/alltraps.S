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

#define ENTERSYS(vector) \
    str r0, [sp, #18]; mov r0, #vector; str r0, [sp, #16]; b enter_sys

spin: b spin
exception_00: b spin // not used
exception_04: // undefined instruction
  sub lr, lr, #4
  ENTERSYS(VCT_UND)
exception_08: // supervisor call
  ENTERSYS(VCT_SVC)
exception_0C: // prefetch abort
  sub lr, lr, #4
  ENTERSYS(VCT_PABT)
exception_10: // data abort
  sub lr, lr, #8
  ENTERSYS(VCT_DABT)
exception_14: b spin // not used
exception_18: // IRQ interrupt
  sub lr, lr, #4
  ENTERSYS(VCT_IRQ)
exception_1C: // FIQ interrupt
  sub lr, lr, #4
  ENTERSYS(VCT_FIQ)

enter_sys:
  //            0       1       2       3     4       5
  //            sp_usr, lr_usr, lr(pc), spsr, vector, r0
  //            ^SP     +4      +8      +12   +16     +18
  mrs r0, spsr; str r0, [sp, #12]
  str lr, [sp, #8]
  mrs r0, lr_usr; str r0, [sp, #4]
  mrs r0, sp_usr; str r0, [sp]
  mov r0, sp
  // enter SYS
  msr cpsr_c, 0xDF
  // SYS mode entered, now we can use lr, sp (r0 is used for pointer to temp frame)
  ldr lr, [r0, #12]
  and lr, #MODE_MASK
  cmp lr, #MODE_SYS
  beq stack_switched
  b spin//FIXME
stack_switched:
  sub sp, #(18*4) // sizeof(struct trap_frame)
  mov lr, r0 // lr is used for pointer to temp frame now
  ldr r0, [lr, #18] // restore r0 to it's original value
  stmia sp, {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12}
  ldmia lr, {r0, r1, r2, r3, r4}
  add lr, sp, #(13 * 4)
  stmia lr, {r0, r1, r2, r3, r4}
  mov r0, sp // arg1 = pointer to trapframe
  bl trap

