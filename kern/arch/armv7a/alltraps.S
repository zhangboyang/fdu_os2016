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
.p2align 5
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
exception_00:
exception_04:
exception_08:
exception_0C:
exception_10:
exception_14:
exception_18:
exception_1C:


alltraps:
mov sp, #0x1000
b trap
