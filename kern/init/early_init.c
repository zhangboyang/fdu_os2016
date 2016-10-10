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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <aim/init.h>

#include <libc/stdio.h>

__noreturn
void master_early_init(void)
{
    bprintf("hello world! this is the AIM kernel!\n");

    bprintf(
        "_____/\\\\\\\\\\\\\\\\\\______/\\\\\\\\\\\\\\\\\\\\\\___/\\\\\\\\____________/\\\\\\\\_        \n"
        " ___/\\\\\\\\\\\\\\\\\\\\\\\\\\___\\/////\\\\\\///___\\/\\\\\\\\\\\\________/\\\\\\\\\\\\_       \n"
        "  __/\\\\\\/////////\\\\\\______\\/\\\\\\______\\/\\\\\\//\\\\\\____/\\\\\\//\\\\\\_      \n"
        "   _\\/\\\\\\_______\\/\\\\\\______\\/\\\\\\______\\/\\\\\\\\///\\\\\\/\\\\\\/_\\/\\\\\\_     \n"
        "    _\\/\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\______\\/\\\\\\______\\/\\\\\\__\\///\\\\\\/___\\/\\\\\\_    \n"
        "     _\\/\\\\\\/////////\\\\\\______\\/\\\\\\______\\/\\\\\\____\\///_____\\/\\\\\\_   \n"
        "      _\\/\\\\\\_______\\/\\\\\\______\\/\\\\\\______\\/\\\\\\_____________\\/\\\\\\_  \n"
        "       _\\/\\\\\\_______\\/\\\\\\___/\\\\\\\\\\\\\\\\\\\\\\__\\/\\\\\\_____________\\/\\\\\\_ \n"
        "        _\\///________\\///___\\///////////___\\///______________\\///__\n\n"

	arch_early_init();
	goto panic;

panic:
	while (1);
}

