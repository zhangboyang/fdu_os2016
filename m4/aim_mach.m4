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

# AIM_SUPPORT_MACH([space_separated_list])
# E.g. if MACH=zynq, define an automake conditional MACH_ZYNQ
AC_DEFUN([AIM_SUPPORT_MACH], [
	m4_foreach_w([var], [$1], [
		AM_CONDITIONAL([MACH_]m4_toupper(var), [test x$MACH = x]var)
	])
])

AC_DEFUN([AIM_INIT_MACH], [
	AIM_ARG_VAR([MACH], [target machine])
	AIM_SUPPORT_MACH([msim loongson3a zynq pc generic unknown])
	AC_SUBST([PREFIXED_MACH], [mach-$MACH])
	AS_CASE([$MACH],
		[zynq], [
			AC_SUBST([CORE], [cortexa9])
			AC_SUBST([PREFIXED_CORE], [core-cortexa9])
			AS_VAR_SET([__with_ram_physbase], [0x00100000])
			AS_VAR_SET([__with_kern_start], [0x00100000])

			AS_VAR_SET([__with_fwstack_order], [12])
			AS_VAR_SET([__enable_uart_lite], [yes])
			AS_VAR_SET([__enable_sd_zynq], [yes])
			AS_VAR_SET([__enable_timer_a9], [yes])

			AS_VAR_SET([__with_primary_console], [UART_LITE])
			AS_VAR_SET([__with_primary_storage], [SD_ZYNQ])
			AS_VAR_SET([__with_primary_timing], [TIMER_A9])

			AS_VAR_SET([__with_cpu_freq], [750000000])
		],
		[msim], [
			AS_VAR_SET([__with_fwstack_order], [12])
			AS_VAR_SET([__enable_uart_msim], [yes])
			AS_VAR_SET([__enable_block_msim], [yes])

			AS_VAR_SET([__with_primary_console], [UART_MSIM])
			AS_VAR_SET([__with_primary_storage], [BLOCK_MSIM])

			AS_VAR_SET([__with_msim_kbd_pa], [0x1f000000])
			AS_VAR_SET([__with_msim_lp_pa], [0x1f000010])
			AS_VAR_SET([__with_msim_disk_pa], [0x1f000020])
			AS_VAR_SET([__with_msim_order_pa], [0x1f000100])
			AS_VAR_SET([__with_msim_mb_base], [0x1f000200])
			AS_VAR_SET([__with_msim_mb_order], [4])
			AS_VAR_SET([__with_msim_rtc_pa], [0x1f000080])
			AS_VAR_SET([__with_msim_firmware_bin], ["firmware.bin"])
			AS_VAR_SET([__with_msim_disk_img], ["disk.img"])
			AS_VAR_SET([__with_cpus], [4])
			AS_VAR_SET([__with_max_cpus], [4])
			AS_VAR_SET([__with_highram_base], [0x80000000])
			AS_VAR_SET([__with_highram_size], [0x00000000])

			AC_DEFINE([CPU_FREQ], [100000000], [CPU frequency])
		], 
		[pc], [
			AS_VAR_SET([__with_fwstack_order], [12])
		],
		[loongson3a], [
			AS_VAR_SET([__with_cpus], [4])
			AS_VAR_SET([__with_max_cpus], [4])
			AS_VAR_SET([__enable_io_port], [yes])
			AS_VAR_SET([__enable_uart_ns16550], [yes])
			AS_VAR_SET([__enable_loongson3a_ram_detection], [yes])
			AS_VAR_SET([__enable_pagesize_16k], [yes])
			AS_VAR_SET([__with_primary_console], [UART_NS16550])
			AS_VAR_SET([__with_cpu_freq], [750000000])
			AS_VAR_SET([__with_kstacksize], [8192])
			AS_VAR_SET([__enable_firmware], [no])
			AS_VAR_SET([__enable_bootloader], [no])
		],
		[generic], [],
		[unknown], [],
		[msim], [
			 AIM_SUBST_MULTILINE([MSIM_CONF_CPULIST],
			 	`for (( i=0; $i<$with_cpus; i=$i+1 )); do \
				echo add dcpu cpu$i; \
				done`
			)
			AC_SUBST(MSIM_CONF_FIRMWARE_BIN, $with_msim_firmware_bin)
			AC_SUBST(MSIM_CONF_KBDPA,
				`printf 0x%08x $with_msim_kbd_pa`)
			AC_SUBST(MSIM_CONF_LPPA,
				`printf 0x%08x $with_msim_lp_pa`)
			AC_SUBST(MSIM_CONF_DISKPA,
				`printf 0x%08x $with_msim_disk_pa`)
			AC_SUBST(MSIM_CONF_DISK_IMAGE, $with_msim_disk_img)
			AC_SUBST(MSIM_CONF_ORDERPA,
				`printf 0x%08x $with_msim_order_pa`)
			AIM_SUBST_MULTILINE(MSIM_CONF_MBLIST,
				`for (( i=0; $i<$with_cpus; i=$i+1 )); do \
				pa=$(( $with_msim_mb_base \
					+ ( $i << $with_msim_mb_order ) )); \
				printf "add rwm mb%d 0x%08x\n" $i $pa; \
				printf "mb%d generic %d\n" $i \
				$(( 1 << $with_msim_mb_order )); \
				done`)
			AC_SUBST(MSIM_CONF_RTCPA,
				`printf 0x%08x $with_msim_rtc_pa`)
			AC_SUBST(MSIM_CONF_HIGHRAM_BASE,
				`if (( $with_highram_size != 0 )); then \
					printf "add rwm highram 0x%08x" \
					$with_highram_base; else \
					echo "";\
				fi`)
			AC_SUBST(MSIM_CONF_HIGHRAM_SIZE,
				`if (( $with_highram_size != 0 )); then \
					printf "highram generic 0x%08x" \
					$with_highram_size; else \
					echo "";\
				fi`)
		],
		[AC_MSG_ERROR([No machine specified])]
	)

])

