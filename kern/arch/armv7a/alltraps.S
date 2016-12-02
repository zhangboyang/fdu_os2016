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
exception_08: // supervisor call
exception_0C: // prefetch abort
  sub lr, lr, #4
exception_10: // data abort
  sub lr, lr, #8
exception_14: b spin // not used
exception_18: // IRQ interrupt
  sub lr, lr, #4
exception_1C: // FIQ interrupt
  sub lr, lr, #4


alltraps:
mov sp, #0x1000
b trap
