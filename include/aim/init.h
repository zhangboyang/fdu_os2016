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

#ifndef _AIM_INIT_H
#define _AIM_INIT_H

#ifndef __ASSEMBLER__

void arch_early_init(void);
void mach_early_init(void);

void mach_master_init(void);

void mach_start_slaves(void);


void slave_early_init(int id);
void arch_slave_early_init(int id);
void mach_slave_early_init(int id);


void slave_early_entry(void);

void master_init(void);
void slave_init(void);

void master_entry(void);
void slave_entry(void);

#endif /* !__ASSEMBLER__ */

#endif /* !_AIM_INIT_H */

