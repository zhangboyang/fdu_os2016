/* Copyright (C) 2016 Gan Quan <coin2028@hotmail.com>
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

#ifndef _DRIVERS_SERIAL_UART_RPI2_HW_H
#define _DRIVERS_SERIAL_UART_RPI2_HW_H

#include <platform.h>

#define GPFSEL1         0x00200004
#define GPSET0          0x0020001C
#define GPCLR0          0x00200028
#define GPPUD           0x00200094
#define GPPUDCLK0       0x00200098

#define AUX_ENABLES     0x00215004
#define AUX_MU_IO_REG   0x00215040
#define AUX_MU_IER_REG  0x00215044
#define AUX_MU_IIR_REG  0x00215048
#define AUX_MU_LCR_REG  0x0021504C
#define AUX_MU_MCR_REG  0x00215050
#define AUX_MU_LSR_REG  0x00215054
#define AUX_MU_MSR_REG  0x00215058
#define AUX_MU_SCRATCH  0x0021505C
#define AUX_MU_CNTL_REG 0x00215060
#define AUX_MU_STAT_REG 0x00215064
#define AUX_MU_BAUD_REG 0x00215068

#endif

