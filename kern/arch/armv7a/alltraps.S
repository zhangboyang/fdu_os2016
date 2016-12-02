/*
    this file is added by ZBY
    construct a trap frame, then call the C routine
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <aim/mmu.h>

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
  
  b trap
