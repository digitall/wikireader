/*
 * file-loader - load an elf file into SDRAM and start it
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

// these items correspond to the load list
#define APPLICATION_TITLE  "Boot WikiReader"
#define APPLICATION_TITLE2 "Boot Forth"
#define APPLICATION_TITLE3 "Boot Calculator"
#define APPLICATION_TITLE4 "Boot Forth (alt)"
#define APPLICATION_TITLE5 "Boot Test Program"
#define APPLICATION_TITLE6 "FLASH MBR"
#define APPLICATION_TITLE7 "FLASH Test Jig"

#include "application.h"
#include "elf32.h"

// each file can have a title above
// so that the menu program can set a start point
static const struct {
	const char *filename;
	int arg;
} LoadList[] = {
	{"kernel.elf", 0},  // status = 0
	{"forth.elf",  0},  // status = 1
	{"calc.elf",   0},  // status = 2
	{"forth.elf",  0},  // status = 3
	{"forth.elf",  1},  // only from boot menu
	{"flash.elf",  0},  // only from boot menu
	{"flash.elf",  1},  // only from boot menu
};

// this must be the first executable code as the loader executes from the first program address
ReturnType file_loader(int block, int status)
{
	APPLICATION_INITIALISE();

	// boot an elf file
	{
		unsigned int i = status;
		if (ARRAY_SIZE(LoadList) <= i) {
			i = 0;
		}
		//for (; i < ARRAY_SIZE(LoadList); ++i) {
		{
			print("load: ");
			print(LoadList[i].filename);
			print_char('\n');
			int error = elf32_exec(LoadList[i].filename, LoadList[i].arg);
			print("load '");
			print(LoadList[i].filename);
			print("' error = ");
			print_int(error);
			print_char('\n');
		}
	}

	APPLICATION_FINALISE(0, 0);
}
