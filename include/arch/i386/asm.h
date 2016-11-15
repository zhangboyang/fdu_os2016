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

#ifndef _ASM_H
#define _ASM_H

static inline
uint8_t inb(uint16_t port)
{
	uint8_t data;
	asm volatile (
		"in %[port], %[data];"
		:
		[data] "=a" (data)	// A reg
		:
		[port] "Nd" (port)	// imm8 or D reg
	);
	return data;
}

static inline
uint16_t inw(uint16_t port)
{
    panic("not implemented!");
	return 0;
}

static inline
uint32_t inl(uint16_t port)
{
    panic("not implemented!");
	return 0;
}

static inline
void insl(int port, void *addr, int cnt)
{
	asm volatile (
		"rep insl;"
		:
		"+D" (addr),	// DI reg
		"+c" (cnt)	// C reg
		:
		"d" (port)	// D reg
		:
		"memory",
		 "cc"
	);
}

static inline
void outb(uint16_t port, uint8_t data)
{
	asm volatile (
		"out %[data], %[port];"
		::
		[data] "a" (data),	// A reg
		[port] "Nd" (port)	// imm8 or D reg
	);
}

static inline void
outw(uint16_t port, uint16_t data)
{
  asm volatile("out %0,%1" : : "a" (data), "d" (port));
}

static inline
void outl(uint16_t port, uint8_t data)
{
    panic("not implemented!");
}

static inline
void stosb(void *addr, int data, int cnt)
{
	asm volatile (
		"rep stosb;"
		:
		"+D" (addr),	// DI reg
		"+c" (cnt)	// C reg
		:
		"a" (data)	// A reg
		:
		"memory",
		"cc"
	);
}







struct segdesc;

static inline void
lgdt(struct segdesc *p, int size)
{
  volatile uint16_t pd[3];

  pd[0] = size-1;
  pd[1] = (uint32_t)p;
  pd[2] = (uint32_t)p >> 16;

  asm volatile("lgdt (%0)" : : "r" (pd));
}

struct gatedesc;

static inline void
lidt(struct gatedesc *p, int size)
{
  volatile uint16_t pd[3];

  pd[0] = size-1;
  pd[1] = (uint32_t)p;
  pd[2] = (uint32_t)p >> 16;

  asm volatile("lidt (%0)" : : "r" (pd));
}

static inline void
ltr(uint16_t sel)
{
  asm volatile("ltr %0" : : "r" (sel));
}

static inline void
loadgs(uint16_t v)
{
  asm volatile("movw %0, %%gs" : : "r" (v));
}

static inline void
cli(void)
{
  asm volatile("cli");
}

static inline void
sti(void)
{
  asm volatile("sti");
}

#endif

