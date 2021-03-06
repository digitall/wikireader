/*
 * interrupt enable/disable
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

#if  !defined(_INTERRUPT_H_)
#define _INTERRUPT_H_ 1

inline void Interrupt_initialise(void)
{
	asm volatile ("psrclr\t4");
}


typedef enum {
	Interrupt_disabled = 0,
	Interrupt_enabled = 1,
} Interrupt_type;


static inline Interrupt_type Interrupt_disable(void)
//+MakeSystemCalls: enter
{
	register int state;
	asm volatile (
		"ld.w\t%0, %%psr\n\t"
		"xand\t%0, 0x010\n\t"
		"sra\t%0, 4\n\t"
		"psrclr\t4"
		: "=r" (state)
		:
		);
	return state;
}
//-MakeSystemCalls: enter

static inline void Interrupt_enable(Interrupt_type state)
//+MakeSystemCalls: exit
{
	if (0 != state) {
		asm volatile ("psrset\t4");
	}
}
//-MakeSystemCalls: exit


#define Interrupt_SaveR15()                             \
	uint32_t saved_r15_value;                       \
	asm volatile ("ld.w\t%[sv], %%r15  \n\t"        \
		      "xld.w\t%%r15, __dp      "        \
		      : [sv] "=r" (saved_r15_value));

#define Interrupt_RestoreR15()                          \
	asm volatile ("ld.w\t%%r15, %[sv]"              \
		      : : [sv] "r"  (saved_r15_value));

#endif
