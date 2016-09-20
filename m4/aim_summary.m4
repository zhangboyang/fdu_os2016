# Copyright (C) 2016 David Gao <davidgao1001@gmail.com>
#
# This file is part of AIM.
#
# AIM is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# AIM is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

AC_DEFUN([AIM_SUMMARY], [AC_MSG_NOTICE([
=========================
  Configuration Summary
=========================

General
--------
  version:	${VERSION}
  build:	${build}
  host:		${host}
  compiler:	${CC}
  CPPFLAGS:	${CPPFLAGS}
  CFLAGS:	${CFLAGS}
  LDFLAGS:	${LDFLAGS}

* Flags are overridden by architecture and machine-specific flags,
* further overridden by per-target flags,
* and further overridden by flags passed to `make`.

Target
--------
  architecture:	${ARCH}
  machine:	${MACH}
  processors:	${with_cpus}/${with_max_cpus}
  memory:	${with_mem_size}

Additional Features
--------
  debug:	${enable_debug}
  tests:	${enable_tests}
  firmware:	${enable_firmware}
  bootloader:	${enable_bootloader}

Kernel
--------
  KERN_BASE:	${with_kern_base}
  KMMAP_BASE:	${with_kmmap_base}
  RESERVED:	${with_reserved_base}
  USERTOP:	${with_usertop}
  USTACKSIZE:	${with_ustacksize}
  Timer freq:	${with_timer_freq}
  Kernel stack:	${with_kstacksize}

Algorithms
--------
  simple allocator:	${with_simple_allocator}
  page allocator:	${with_page_allocator}
  caching allocator:	${with_caching_allocator}
  device index:		${with_dev_index}

Drivers
--------
  io-mem:	${enable_io_mem}
  io-port:	${enable_io_port}
  uart-zynq:	${enable_uart_zynq}
  uart-msim:	${enable_uart_msim}
  uart-ns16550:	${enable_uart_ns16550}
  block-msim:	${enable_block_msim}
  sd-zynq:	${enable_sd_zynq}
  timer-a9:	${enable_timer_a9}

Primary Drivers
--------
  console:	${with_primary_console}
  storage:	${with_primary_storage}
  timing:	${with_primary_timing}

Firmware
--------
  stack order:	${with_fwstack_order}
])])

