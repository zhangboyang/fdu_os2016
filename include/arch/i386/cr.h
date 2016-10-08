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

#ifndef _CR_H
#define _CR_H

#ifdef __ASSEMBLER__

#else /* !__ASSEMBLER__ */

#endif /* !__ASSEMBLER__ */

#define CR0_PE	0x00000001	// Protection Enable
#define CR0_MP	0x00000002	// Monitor coProcessor
#define CR0_EM	0x00000004	// Emulation
#define CR0_TS	0x00000008	// Task Switched
#define CR0_ET	0x00000010	// Extension Type
#define CR0_NE	0x00000020	// Numeric Errror
#define CR0_WP	0x00010000	// Write Protect
#define CR0_AM	0x00040000	// Alignment Mask
#define CR0_NW	0x20000000	// Not Writethrough
#define CR0_CD	0x40000000	// Cache Disable
#define CR0_PG	0x80000000	// Paging

#define CR4_PSE	0x00000010	// Page size extension

#endif /* !_CR_H */

