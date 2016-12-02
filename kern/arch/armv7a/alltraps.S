/*
    this file is added by ZBY
    construct a trap frame, then call the C routine
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <aim/mmu.h>

// exception vectors
.data
.align 32
.global vectors
vectors:
.long exception_00
.long exception_04
.long exception_08
.long exception_0C
.long exception_10
.long exception_14
.long exception_18
.long exception_1C


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
b trap
