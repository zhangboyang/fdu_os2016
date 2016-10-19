/* Copyright (C) 1994, 1995 by Ralf Baechle
 * Copyright (C) 2016 Gan Quan <coin2028@hotmail.com>
 *
 * This file is part of AIMv6.
 *
 * AIMv6 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * AIMv6 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __ASM_MIPS_REGDEF_H
#define __ASM_MIPS_REGDEF_H

/*
 * Symbolic register names for 32 bit ABI
 * 
 * Although I'm programming on a 64-bit machine, I retain the 32 bit register
 * names.
 */
#define zero	$0	/* wired zero */
#define AT	$1	/* assembler temp  - uppercase because of ".set at" */
#define v0	$2	/* return value */
#define v1	$3
#define a0	$4	/* argument registers */
#define a1	$5
#define a2	$6
#define a3	$7
#define t0	$8	/* t0-t3 are argument registers in MIPS64 ABI */
#define t1	$9
#define t2	$10
#define t3	$11
#define a4	$8	/* MIPS64 ABI aliases for t0-t3 */
#define a5	$9
#define a6	$10
#define a7	$11
#define t4	$12	/* caller saved */
#define t5	$13
#define t6	$14
#define t7	$15
#define s0	$16	/* callee saved */
#define s1	$17
#define s2	$18
#define s3	$19
#define s4	$20
#define s5	$21
#define s6	$22
#define s7	$23
#define t8	$24
#define t9	$25
#define jp	$26
#define k0	$26
#define k1	$27
#define gp	$28
#define sp	$29
#define fp	$30
#define s8	$30
#define ra	$31

#endif /* __ASM_MIPS_REGDEF_H */

