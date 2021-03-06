/*
 * mbr - FLASH ROM based application header
 *
 * Copyright (c) 2009 Openmoko Inc.
 *
 * Authors   Christopher Hall <hsw@openmoko.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#if !defined(_APPLICATION_H_)
#define _APPLICATION_H_

#include <inttypes.h>
#include <stdlib.h>
#include <stdbool.h>

#include <regs.h>
#include <samo.h>
#include <delay.h>
#include <print.h>
#include <console.h>

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof((a)) / sizeof((a)[0]))
#endif

#if !defined(APPLICATION_TITLE)
#error "APPLICATION_TITLE must be defined before including application.h"
#endif

// setup and configure run time environment
// the very first line of main() after the '}'
#define APPLICATION_INITIALISE()				\
	do {							\
		asm volatile ("\txld.w\t%r15, __dp\n"		\
			      "\txld.w\t%r10, __SIZE_bss\n"	\
			      "\tor\t%r10, %r10\n"		\
			      "\tjreq\tclear_bss_done\n"	\
			      "\txld.w\t%r9, __START_bss\n"	\
			      "\txor\t%r8, %r8\n"		\
			      "clear_bss_loop:\n"		\
			      "\tld.w\t[%r9]+, %r8\n"		\
			      "\tsub\t%r10, 4\n"		\
			      "\tjrgt\tclear_bss_loop\n"	\
			      "clear_bss_done:\n"		\
			);					\
		init_pins();					\
		init_rs232_ch0();				\
		init_rs232_ch1();				\
		init_ram();					\
	} while (0)


// the last line of main() before the final '}'
// at present just returns the next_program number to
#define APPLICATION_FINALISE(next_program, status)	\
	do {						\
		ReturnType rc = {next_program, status}; \
		return (rc);				\
	} while (0)


__attribute__ ((packed))
typedef struct {
	unsigned int block;
	unsigned int status;
} ReturnType;

typedef ReturnType application(int block, int status);


#endif
