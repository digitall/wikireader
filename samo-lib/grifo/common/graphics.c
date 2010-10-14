/*
 * Generic graphics routines
 *
 * Copyright (c) 2010 Openmoko Inc.
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

#include "standard.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "graphics.h"


// Bresenham's line algorithm
void Graphics_DrawLine(int x0, int y0, int x1, int y1, uint32_t colour,
	      void (*SetPixel)(int x, int y, uint32_t colour))
{
	int stepx = 1;
	int dx = (x1 - x0) << 1;

	if (dx < 0) {
		dx = -dx;
		stepx = -1;
	}

	int stepy = 1;
	int dy = (y1 - y0) << 1;

	if (dy < 0) {
		dy = -dy;
		stepy = -1;
	}

	SetPixel(x0, y0, colour);

	if (dx > dy) {
		int fraction = dy - (dx >> 1);

		while (x0 != x1) {
			if (fraction >= 0) {
				y0 += stepy;
				fraction -= dx;
			}
			x0 += stepx;
			fraction += dy;
			SetPixel(x0, y0, colour);
		}
	} else {
		int fraction = dx - (dy >> 1);

		while (y0 != y1) {
			if (fraction >= 0) {
				x0 += stepx;
				fraction -= dy;
			}
			y0 += stepy;
			fraction += dx;
			SetPixel(x0, y0, colour);
		}
	}
}


void Graphics_PutBitMap(void *framebuffer, size_t BufferWidth,
			int x, int y, size_t width, size_t height,
			bool reverse,
			const uint8_t *bits)
{
	uint8_t *p = (uint8_t *)framebuffer + (x >> 3) + (BufferWidth * y);
	size_t h;

	uint8_t m = reverse ? 0xff : 0;
	width >>= 3;
	if (0 == width || 0 == height) {
		return;
	}
	for (h = 0; h < height; ++h) {
		size_t w;

		for (w = 0; w < width; ++w) {
			p[w] = m ^ *bits++;
		}
		p += BufferWidth;
	}
}

#define FONT_WIDTH 8
#define FONT_HEIGHT 13

static const uint8_t font_8x13[256][FONT_HEIGHT] = {
	[0x0000] { 0x00, 0x00, 0xAA, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0xAA, 0x00, 0x00 },
	[0x0001] { 0x00, 0x00, 0x00, 0x10, 0x38, 0x7C, 0xFE, 0x7C, 0x38, 0x10, 0x00, 0x00, 0x00 },
	[0x0002] { 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA },
	[0x0003] { 0x00, 0x00, 0xA0, 0xA0, 0xE0, 0xA0, 0xAE, 0x04, 0x04, 0x04, 0x04, 0x00, 0x00 },
	[0x0004] { 0x00, 0x00, 0xE0, 0x80, 0xC0, 0x80, 0x8E, 0x08, 0x0C, 0x08, 0x08, 0x00, 0x00 },
	[0x0005] { 0x00, 0x00, 0x60, 0x80, 0x80, 0x80, 0x6C, 0x0A, 0x0C, 0x0A, 0x0A, 0x00, 0x00 },
	[0x0006] { 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0xEE, 0x08, 0x0C, 0x08, 0x08, 0x00, 0x00 },
	[0x0007] { 0x00, 0x00, 0x18, 0x24, 0x24, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x0008] { 0x00, 0x00, 0x00, 0x10, 0x10, 0x7C, 0x10, 0x10, 0x00, 0x7C, 0x00, 0x00, 0x00 },
	[0x0009] { 0x00, 0x00, 0xC0, 0xA0, 0xA0, 0xA0, 0xA8, 0x08, 0x08, 0x08, 0x0E, 0x00, 0x00 },
	[0x000A] { 0x00, 0x00, 0x88, 0x88, 0x50, 0x50, 0x2E, 0x04, 0x04, 0x04, 0x04, 0x00, 0x00 },
	[0x000B] { 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x000C] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10 },
	[0x000D] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10 },
	[0x000E] { 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x000F] { 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0xFF, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10 },
	[0x0010] { 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x0011] { 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x0012] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x0013] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00 },
	[0x0014] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF },
	[0x0015] { 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10 },
	[0x0016] { 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0xF0, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10 },
	[0x0017] { 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x0018] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10 },
	[0x0019] { 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10 },
	[0x001A] { 0x00, 0x00, 0x00, 0x00, 0x0E, 0x30, 0xC0, 0x30, 0x0E, 0x00, 0xFE, 0x00, 0x00 },
	[0x001B] { 0x00, 0x00, 0x00, 0x00, 0xE0, 0x18, 0x06, 0x18, 0xE0, 0x00, 0xFE, 0x00, 0x00 },
	[0x001C] { 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0x44, 0x44, 0x44, 0x44, 0x44, 0x00, 0x00 },
	[0x001D] { 0x00, 0x00, 0x00, 0x04, 0x04, 0x7E, 0x08, 0x10, 0x7E, 0x20, 0x20, 0x00, 0x00 },
	[0x001E] { 0x00, 0x00, 0x1C, 0x22, 0x20, 0x70, 0x20, 0x20, 0x20, 0x62, 0xDC, 0x00, 0x00 },
	[0x001F] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },

	// printable ascii codes
	[0x0020] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x0021] { 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x10, 0x00, 0x00 },
	[0x0022] { 0x00, 0x00, 0x24, 0x24, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x0023] { 0x00, 0x00, 0x00, 0x24, 0x24, 0x7E, 0x24, 0x7E, 0x24, 0x24, 0x00, 0x00, 0x00 },
	[0x0024] { 0x00, 0x00, 0x10, 0x3C, 0x50, 0x50, 0x38, 0x14, 0x14, 0x78, 0x10, 0x00, 0x00 },
	[0x0025] { 0x00, 0x00, 0x22, 0x52, 0x24, 0x08, 0x08, 0x10, 0x24, 0x2A, 0x44, 0x00, 0x00 },
	[0x0026] { 0x00, 0x00, 0x00, 0x00, 0x30, 0x48, 0x48, 0x30, 0x4A, 0x44, 0x3A, 0x00, 0x00 },
	[0x0027] { 0x00, 0x00, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x0028] { 0x00, 0x00, 0x04, 0x08, 0x08, 0x10, 0x10, 0x10, 0x08, 0x08, 0x04, 0x00, 0x00 },
	[0x0029] { 0x00, 0x00, 0x20, 0x10, 0x10, 0x08, 0x08, 0x08, 0x10, 0x10, 0x20, 0x00, 0x00 },
	[0x002A] { 0x00, 0x00, 0x24, 0x18, 0x7E, 0x18, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x002B] { 0x00, 0x00, 0x00, 0x00, 0x10, 0x10, 0x7C, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00 },
	[0x002C] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x30, 0x40, 0x00 },
	[0x002D] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x002E] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x38, 0x10, 0x00 },
	[0x002F] { 0x00, 0x00, 0x02, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x80, 0x00, 0x00 },
	[0x0030] { 0x00, 0x00, 0x18, 0x24, 0x42, 0x42, 0x42, 0x42, 0x42, 0x24, 0x18, 0x00, 0x00 },
	[0x0031] { 0x00, 0x00, 0x10, 0x30, 0x50, 0x10, 0x10, 0x10, 0x10, 0x10, 0x7C, 0x00, 0x00 },
	[0x0032] { 0x00, 0x00, 0x3C, 0x42, 0x42, 0x02, 0x04, 0x18, 0x20, 0x40, 0x7E, 0x00, 0x00 },
	[0x0033] { 0x00, 0x00, 0x7E, 0x02, 0x04, 0x08, 0x1C, 0x02, 0x02, 0x42, 0x3C, 0x00, 0x00 },
	[0x0034] { 0x00, 0x00, 0x04, 0x0C, 0x14, 0x24, 0x44, 0x44, 0x7E, 0x04, 0x04, 0x00, 0x00 },
	[0x0035] { 0x00, 0x00, 0x7E, 0x40, 0x40, 0x5C, 0x62, 0x02, 0x02, 0x42, 0x3C, 0x00, 0x00 },
	[0x0036] { 0x00, 0x00, 0x1C, 0x20, 0x40, 0x40, 0x5C, 0x62, 0x42, 0x42, 0x3C, 0x00, 0x00 },
	[0x0037] { 0x00, 0x00, 0x7E, 0x02, 0x04, 0x08, 0x08, 0x10, 0x10, 0x20, 0x20, 0x00, 0x00 },
	[0x0038] { 0x00, 0x00, 0x3C, 0x42, 0x42, 0x42, 0x3C, 0x42, 0x42, 0x42, 0x3C, 0x00, 0x00 },
	[0x0039] { 0x00, 0x00, 0x3C, 0x42, 0x42, 0x46, 0x3A, 0x02, 0x02, 0x04, 0x38, 0x00, 0x00 },
	[0x003A] { 0x00, 0x00, 0x00, 0x00, 0x10, 0x38, 0x10, 0x00, 0x00, 0x10, 0x38, 0x10, 0x00 },
	[0x003B] { 0x00, 0x00, 0x00, 0x00, 0x10, 0x38, 0x10, 0x00, 0x00, 0x38, 0x30, 0x40, 0x00 },
	[0x003C] { 0x00, 0x00, 0x02, 0x04, 0x08, 0x10, 0x20, 0x10, 0x08, 0x04, 0x02, 0x00, 0x00 },
	[0x003D] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00 },
	[0x003E] { 0x00, 0x00, 0x40, 0x20, 0x10, 0x08, 0x04, 0x08, 0x10, 0x20, 0x40, 0x00, 0x00 },
	[0x003F] { 0x00, 0x00, 0x3C, 0x42, 0x42, 0x02, 0x04, 0x08, 0x08, 0x00, 0x08, 0x00, 0x00 },
	[0x0040] { 0x00, 0x00, 0x3C, 0x42, 0x42, 0x4E, 0x52, 0x56, 0x4A, 0x40, 0x3C, 0x00, 0x00 },
	[0x0041] { 0x00, 0x00, 0x18, 0x24, 0x42, 0x42, 0x42, 0x7E, 0x42, 0x42, 0x42, 0x00, 0x00 },
	[0x0042] { 0x00, 0x00, 0x78, 0x44, 0x42, 0x44, 0x78, 0x44, 0x42, 0x44, 0x78, 0x00, 0x00 },
	[0x0043] { 0x00, 0x00, 0x3C, 0x42, 0x40, 0x40, 0x40, 0x40, 0x40, 0x42, 0x3C, 0x00, 0x00 },
	[0x0044] { 0x00, 0x00, 0x78, 0x44, 0x42, 0x42, 0x42, 0x42, 0x42, 0x44, 0x78, 0x00, 0x00 },
	[0x0045] { 0x00, 0x00, 0x7E, 0x40, 0x40, 0x40, 0x78, 0x40, 0x40, 0x40, 0x7E, 0x00, 0x00 },
	[0x0046] { 0x00, 0x00, 0x7E, 0x40, 0x40, 0x40, 0x78, 0x40, 0x40, 0x40, 0x40, 0x00, 0x00 },
	[0x0047] { 0x00, 0x00, 0x3C, 0x42, 0x40, 0x40, 0x40, 0x4E, 0x42, 0x46, 0x3A, 0x00, 0x00 },
	[0x0048] { 0x00, 0x00, 0x42, 0x42, 0x42, 0x42, 0x7E, 0x42, 0x42, 0x42, 0x42, 0x00, 0x00 },
	[0x0049] { 0x00, 0x00, 0x7C, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x7C, 0x00, 0x00 },
	[0x004A] { 0x00, 0x00, 0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x44, 0x38, 0x00, 0x00 },
	[0x004B] { 0x00, 0x00, 0x42, 0x44, 0x48, 0x50, 0x60, 0x50, 0x48, 0x44, 0x42, 0x00, 0x00 },
	[0x004C] { 0x00, 0x00, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x7E, 0x00, 0x00 },
	[0x004D] { 0x00, 0x00, 0x82, 0x82, 0xC6, 0xAA, 0x92, 0x92, 0x82, 0x82, 0x82, 0x00, 0x00 },
	[0x004E] { 0x00, 0x00, 0x42, 0x42, 0x62, 0x52, 0x4A, 0x46, 0x42, 0x42, 0x42, 0x00, 0x00 },
	[0x004F] { 0x00, 0x00, 0x3C, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3C, 0x00, 0x00 },
	[0x0050] { 0x00, 0x00, 0x7C, 0x42, 0x42, 0x42, 0x7C, 0x40, 0x40, 0x40, 0x40, 0x00, 0x00 },
	[0x0051] { 0x00, 0x00, 0x3C, 0x42, 0x42, 0x42, 0x42, 0x42, 0x52, 0x4A, 0x3C, 0x02, 0x00 },
	[0x0052] { 0x00, 0x00, 0x7C, 0x42, 0x42, 0x42, 0x7C, 0x50, 0x48, 0x44, 0x42, 0x00, 0x00 },
	[0x0053] { 0x00, 0x00, 0x3C, 0x42, 0x40, 0x40, 0x3C, 0x02, 0x02, 0x42, 0x3C, 0x00, 0x00 },
	[0x0054] { 0x00, 0x00, 0xFE, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00 },
	[0x0055] { 0x00, 0x00, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3C, 0x00, 0x00 },
	[0x0056] { 0x00, 0x00, 0x82, 0x82, 0x44, 0x44, 0x44, 0x28, 0x28, 0x28, 0x10, 0x00, 0x00 },
	[0x0057] { 0x00, 0x00, 0x82, 0x82, 0x82, 0x82, 0x92, 0x92, 0x92, 0xAA, 0x44, 0x00, 0x00 },
	[0x0058] { 0x00, 0x00, 0x82, 0x82, 0x44, 0x28, 0x10, 0x28, 0x44, 0x82, 0x82, 0x00, 0x00 },
	[0x0059] { 0x00, 0x00, 0x82, 0x82, 0x44, 0x28, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00 },
	[0x005A] { 0x00, 0x00, 0x7E, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x40, 0x7E, 0x00, 0x00 },
	[0x005B] { 0x00, 0x00, 0x3C, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x3C, 0x00, 0x00 },
	[0x005C] { 0x00, 0x00, 0x80, 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x02, 0x00, 0x00 },
	[0x005D] { 0x00, 0x00, 0x78, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x78, 0x00, 0x00 },
	[0x005E] { 0x00, 0x00, 0x10, 0x28, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x005F] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0x00 },
	[0x0060] { 0x00, 0x10, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x0061] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x02, 0x3E, 0x42, 0x46, 0x3A, 0x00, 0x00 },
	[0x0062] { 0x00, 0x00, 0x40, 0x40, 0x40, 0x5C, 0x62, 0x42, 0x42, 0x62, 0x5C, 0x00, 0x00 },
	[0x0063] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x42, 0x40, 0x40, 0x42, 0x3C, 0x00, 0x00 },
	[0x0064] { 0x00, 0x00, 0x02, 0x02, 0x02, 0x3A, 0x46, 0x42, 0x42, 0x46, 0x3A, 0x00, 0x00 },
	[0x0065] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x42, 0x7E, 0x40, 0x42, 0x3C, 0x00, 0x00 },
	[0x0066] { 0x00, 0x00, 0x1C, 0x22, 0x20, 0x20, 0x7C, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00 },
	[0x0067] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x3A, 0x44, 0x44, 0x38, 0x40, 0x3C, 0x42, 0x3C },
	[0x0068] { 0x00, 0x00, 0x40, 0x40, 0x40, 0x5C, 0x62, 0x42, 0x42, 0x42, 0x42, 0x00, 0x00 },
	[0x0069] { 0x00, 0x00, 0x00, 0x10, 0x00, 0x30, 0x10, 0x10, 0x10, 0x10, 0x7C, 0x00, 0x00 },
	[0x006A] { 0x00, 0x00, 0x00, 0x04, 0x00, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x44, 0x44, 0x38 },
	[0x006B] { 0x00, 0x00, 0x40, 0x40, 0x40, 0x44, 0x48, 0x70, 0x48, 0x44, 0x42, 0x00, 0x00 },
	[0x006C] { 0x00, 0x00, 0x30, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x7C, 0x00, 0x00 },
	[0x006D] { 0x00, 0x00, 0x00, 0x00, 0x00, 0xEC, 0x92, 0x92, 0x92, 0x92, 0x82, 0x00, 0x00 },
	[0x006E] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x5C, 0x62, 0x42, 0x42, 0x42, 0x42, 0x00, 0x00 },
	[0x006F] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x42, 0x42, 0x42, 0x42, 0x3C, 0x00, 0x00 },
	[0x0070] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x5C, 0x62, 0x42, 0x62, 0x5C, 0x40, 0x40, 0x40 },
	[0x0071] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x3A, 0x46, 0x42, 0x46, 0x3A, 0x02, 0x02, 0x02 },
	[0x0072] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x5C, 0x22, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00 },
	[0x0073] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x42, 0x30, 0x0C, 0x42, 0x3C, 0x00, 0x00 },
	[0x0074] { 0x00, 0x00, 0x00, 0x20, 0x20, 0x7C, 0x20, 0x20, 0x20, 0x22, 0x1C, 0x00, 0x00 },
	[0x0075] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0x44, 0x44, 0x44, 0x44, 0x3A, 0x00, 0x00 },
	[0x0076] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0x44, 0x44, 0x28, 0x28, 0x10, 0x00, 0x00 },
	[0x0077] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x82, 0x82, 0x92, 0x92, 0xAA, 0x44, 0x00, 0x00 },
	[0x0078] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x00, 0x00 },
	[0x0079] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x42, 0x42, 0x46, 0x3A, 0x02, 0x42, 0x3C },
	[0x007A] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x04, 0x08, 0x10, 0x20, 0x7E, 0x00, 0x00 },
	[0x007B] { 0x00, 0x00, 0x0E, 0x10, 0x10, 0x08, 0x30, 0x08, 0x10, 0x10, 0x0E, 0x00, 0x00 },
	[0x007C] { 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00 },
	[0x007D] { 0x00, 0x00, 0x70, 0x08, 0x08, 0x10, 0x0C, 0x10, 0x08, 0x08, 0x70, 0x00, 0x00 },
	[0x007E] { 0x00, 0x00, 0x24, 0x54, 0x48, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },

	// spare codes
	[0x007F] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x0080] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x0081] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x0082] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x0083] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x0084] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x0085] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x0086] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x0087] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x0088] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x0089] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x008A] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x008B] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x008C] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x008D] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x008E] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x008F] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x0090] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x0091] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x0092] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x0093] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x0094] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x0095] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x0096] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x0097] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x0098] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x0099] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x009A] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x009B] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x009C] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x009D] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x009E] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x009F] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },

	// some special codes and accented letters
	[0x00A0] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x00A1] { 0x00, 0x00, 0x10, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00 },
	[0x00A2] { 0x00, 0x00, 0x10, 0x38, 0x54, 0x50, 0x50, 0x54, 0x38, 0x10, 0x00, 0x00, 0x00 },
	[0x00A3] { 0x00, 0x00, 0x1C, 0x22, 0x20, 0x70, 0x20, 0x20, 0x20, 0x62, 0xDC, 0x00, 0x00 },
	[0x00A4] { 0x00, 0x00, 0x00, 0x00, 0x42, 0x3C, 0x24, 0x24, 0x3C, 0x42, 0x00, 0x00, 0x00 },
	[0x00A5] { 0x00, 0x00, 0x82, 0x82, 0x44, 0x28, 0x7C, 0x10, 0x7C, 0x10, 0x10, 0x00, 0x00 },
	[0x00A6] { 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x00, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00 },
	[0x00A7] { 0x00, 0x18, 0x24, 0x20, 0x18, 0x24, 0x24, 0x18, 0x04, 0x24, 0x18, 0x00, 0x00 },
	[0x00A8] { 0x00, 0x24, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x00A9] { 0x00, 0x38, 0x44, 0x92, 0xAA, 0xA2, 0xAA, 0x92, 0x44, 0x38, 0x00, 0x00, 0x00 },
	[0x00AA] { 0x00, 0x00, 0x38, 0x04, 0x3C, 0x44, 0x3C, 0x00, 0x7C, 0x00, 0x00, 0x00, 0x00 },
	[0x00AB] { 0x00, 0x00, 0x00, 0x12, 0x24, 0x48, 0x90, 0x48, 0x24, 0x12, 0x00, 0x00, 0x00 },
	[0x00AC] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x02, 0x02, 0x02, 0x00, 0x00, 0x00 },
	[0x00AD] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x00AE] { 0x00, 0x38, 0x44, 0x92, 0xAA, 0xAA, 0xB2, 0xAA, 0x44, 0x38, 0x00, 0x00, 0x00 },
	[0x00AF] { 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x00B0] { 0x00, 0x00, 0x18, 0x24, 0x24, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x00B1] { 0x00, 0x00, 0x00, 0x10, 0x10, 0x7C, 0x10, 0x10, 0x00, 0x7C, 0x00, 0x00, 0x00 },
	[0x00B2] { 0x00, 0x30, 0x48, 0x08, 0x30, 0x40, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x00B3] { 0x00, 0x30, 0x48, 0x10, 0x08, 0x48, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x00B4] { 0x00, 0x08, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x00B5] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x42, 0x42, 0x42, 0x66, 0x5A, 0x40, 0x00 },
	[0x00B6] { 0x00, 0x00, 0x3E, 0x74, 0x74, 0x74, 0x34, 0x14, 0x14, 0x14, 0x14, 0x00, 0x00 },
	[0x00B7] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x00B8] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x18 },
	[0x00B9] { 0x00, 0x20, 0x60, 0x20, 0x20, 0x20, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x00BA] { 0x00, 0x00, 0x30, 0x48, 0x48, 0x30, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00 },
	[0x00BB] { 0x00, 0x00, 0x00, 0x90, 0x48, 0x24, 0x12, 0x24, 0x48, 0x90, 0x00, 0x00, 0x00 },
	[0x00BC] { 0x00, 0x40, 0xC0, 0x40, 0x40, 0x42, 0xE6, 0x0A, 0x12, 0x1A, 0x06, 0x00, 0x00 },
	[0x00BD] { 0x00, 0x40, 0xC0, 0x40, 0x40, 0x4C, 0xF2, 0x02, 0x0C, 0x10, 0x1E, 0x00, 0x00 },
	[0x00BE] { 0x00, 0x60, 0x90, 0x20, 0x10, 0x92, 0x66, 0x0A, 0x12, 0x1A, 0x06, 0x00, 0x00 },
	[0x00BF] { 0x00, 0x00, 0x10, 0x00, 0x10, 0x10, 0x20, 0x40, 0x42, 0x42, 0x3C, 0x00, 0x00 },
	[0x00C0] { 0x00, 0x10, 0x08, 0x00, 0x18, 0x24, 0x42, 0x42, 0x7E, 0x42, 0x42, 0x00, 0x00 },
	[0x00C1] { 0x00, 0x08, 0x10, 0x00, 0x18, 0x24, 0x42, 0x42, 0x7E, 0x42, 0x42, 0x00, 0x00 },
	[0x00C2] { 0x00, 0x18, 0x24, 0x00, 0x18, 0x24, 0x42, 0x42, 0x7E, 0x42, 0x42, 0x00, 0x00 },
	[0x00C3] { 0x00, 0x32, 0x4C, 0x00, 0x18, 0x24, 0x42, 0x42, 0x7E, 0x42, 0x42, 0x00, 0x00 },
	[0x00C4] { 0x00, 0x24, 0x24, 0x00, 0x18, 0x24, 0x42, 0x42, 0x7E, 0x42, 0x42, 0x00, 0x00 },
	[0x00C5] { 0x00, 0x18, 0x24, 0x18, 0x18, 0x24, 0x42, 0x42, 0x7E, 0x42, 0x42, 0x00, 0x00 },
	[0x00C6] { 0x00, 0x00, 0x6E, 0x90, 0x90, 0x90, 0x9C, 0xF0, 0x90, 0x90, 0x9E, 0x00, 0x00 },
	[0x00C7] { 0x00, 0x00, 0x3C, 0x42, 0x40, 0x40, 0x40, 0x40, 0x40, 0x42, 0x3C, 0x08, 0x10 },
	[0x00C8] { 0x00, 0x10, 0x08, 0x00, 0x7E, 0x40, 0x40, 0x78, 0x40, 0x40, 0x7E, 0x00, 0x00 },
	[0x00C9] { 0x00, 0x08, 0x10, 0x00, 0x7E, 0x40, 0x40, 0x78, 0x40, 0x40, 0x7E, 0x00, 0x00 },
	[0x00CA] { 0x00, 0x18, 0x24, 0x00, 0x7E, 0x40, 0x40, 0x78, 0x40, 0x40, 0x7E, 0x00, 0x00 },
	[0x00CB] { 0x00, 0x24, 0x24, 0x00, 0x7E, 0x40, 0x40, 0x78, 0x40, 0x40, 0x7E, 0x00, 0x00 },
	[0x00CC] { 0x00, 0x20, 0x10, 0x00, 0x7C, 0x10, 0x10, 0x10, 0x10, 0x10, 0x7C, 0x00, 0x00 },
	[0x00CD] { 0x00, 0x08, 0x10, 0x00, 0x7C, 0x10, 0x10, 0x10, 0x10, 0x10, 0x7C, 0x00, 0x00 },
	[0x00CE] { 0x00, 0x18, 0x24, 0x00, 0x7C, 0x10, 0x10, 0x10, 0x10, 0x10, 0x7C, 0x00, 0x00 },
	[0x00CF] { 0x00, 0x44, 0x44, 0x00, 0x7C, 0x10, 0x10, 0x10, 0x10, 0x10, 0x7C, 0x00, 0x00 },
	[0x00D0] { 0x00, 0x00, 0x78, 0x44, 0x42, 0x42, 0xE2, 0x42, 0x42, 0x44, 0x78, 0x00, 0x00 },
	[0x00D1] { 0x00, 0x64, 0x98, 0x00, 0x82, 0xC2, 0xA2, 0x92, 0x8A, 0x86, 0x82, 0x00, 0x00 },
	[0x00D2] { 0x00, 0x20, 0x10, 0x00, 0x7C, 0x82, 0x82, 0x82, 0x82, 0x82, 0x7C, 0x00, 0x00 },
	[0x00D3] { 0x00, 0x08, 0x10, 0x00, 0x7C, 0x82, 0x82, 0x82, 0x82, 0x82, 0x7C, 0x00, 0x00 },
	[0x00D4] { 0x00, 0x18, 0x24, 0x00, 0x7C, 0x82, 0x82, 0x82, 0x82, 0x82, 0x7C, 0x00, 0x00 },
	[0x00D5] { 0x00, 0x64, 0x98, 0x00, 0x7C, 0x82, 0x82, 0x82, 0x82, 0x82, 0x7C, 0x00, 0x00 },
	[0x00D6] { 0x00, 0x44, 0x44, 0x00, 0x7C, 0x82, 0x82, 0x82, 0x82, 0x82, 0x7C, 0x00, 0x00 },
	[0x00D7] { 0x00, 0x00, 0x00, 0x00, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x00, 0x00, 0x00 },
	[0x00D8] { 0x00, 0x02, 0x3C, 0x46, 0x4A, 0x4A, 0x52, 0x52, 0x52, 0x62, 0x3C, 0x40, 0x00 },
	[0x00D9] { 0x00, 0x20, 0x10, 0x00, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3C, 0x00, 0x00 },
	[0x00DA] { 0x00, 0x08, 0x10, 0x00, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3C, 0x00, 0x00 },
	[0x00DB] { 0x00, 0x18, 0x24, 0x00, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3C, 0x00, 0x00 },
	[0x00DC] { 0x00, 0x24, 0x24, 0x00, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3C, 0x00, 0x00 },
	[0x00DD] { 0x00, 0x08, 0x10, 0x00, 0x44, 0x44, 0x28, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00 },
	[0x00DE] { 0x00, 0x00, 0x40, 0x7C, 0x42, 0x42, 0x42, 0x7C, 0x40, 0x40, 0x40, 0x00, 0x00 },
	[0x00DF] { 0x00, 0x00, 0x38, 0x44, 0x44, 0x48, 0x50, 0x4C, 0x42, 0x42, 0x5C, 0x00, 0x00 },
	[0x00E0] { 0x00, 0x00, 0x10, 0x08, 0x00, 0x3C, 0x02, 0x3E, 0x42, 0x46, 0x3A, 0x00, 0x00 },
	[0x00E1] { 0x00, 0x00, 0x04, 0x08, 0x00, 0x3C, 0x02, 0x3E, 0x42, 0x46, 0x3A, 0x00, 0x00 },
	[0x00E2] { 0x00, 0x00, 0x18, 0x24, 0x00, 0x3C, 0x02, 0x3E, 0x42, 0x46, 0x3A, 0x00, 0x00 },
	[0x00E3] { 0x00, 0x00, 0x32, 0x4C, 0x00, 0x3C, 0x02, 0x3E, 0x42, 0x46, 0x3A, 0x00, 0x00 },
	[0x00E4] { 0x00, 0x00, 0x24, 0x24, 0x00, 0x3C, 0x02, 0x3E, 0x42, 0x46, 0x3A, 0x00, 0x00 },
	[0x00E5] { 0x00, 0x18, 0x24, 0x18, 0x00, 0x3C, 0x02, 0x3E, 0x42, 0x46, 0x3A, 0x00, 0x00 },
	[0x00E6] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x6C, 0x12, 0x7C, 0x90, 0x92, 0x6C, 0x00, 0x00 },
	[0x00E7] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x42, 0x40, 0x40, 0x42, 0x3C, 0x08, 0x10 },
	[0x00E8] { 0x00, 0x00, 0x10, 0x08, 0x00, 0x3C, 0x42, 0x7E, 0x40, 0x42, 0x3C, 0x00, 0x00 },
	[0x00E9] { 0x00, 0x00, 0x08, 0x10, 0x00, 0x3C, 0x42, 0x7E, 0x40, 0x42, 0x3C, 0x00, 0x00 },
	[0x00EA] { 0x00, 0x00, 0x18, 0x24, 0x00, 0x3C, 0x42, 0x7E, 0x40, 0x42, 0x3C, 0x00, 0x00 },
	[0x00EB] { 0x00, 0x00, 0x24, 0x24, 0x00, 0x3C, 0x42, 0x7E, 0x40, 0x42, 0x3C, 0x00, 0x00 },
	[0x00EC] { 0x00, 0x00, 0x20, 0x10, 0x00, 0x30, 0x10, 0x10, 0x10, 0x10, 0x7C, 0x00, 0x00 },
	[0x00ED] { 0x00, 0x00, 0x10, 0x20, 0x00, 0x30, 0x10, 0x10, 0x10, 0x10, 0x7C, 0x00, 0x00 },
	[0x00EE] { 0x00, 0x00, 0x30, 0x48, 0x00, 0x30, 0x10, 0x10, 0x10, 0x10, 0x7C, 0x00, 0x00 },
	[0x00EF] { 0x00, 0x00, 0x48, 0x48, 0x00, 0x30, 0x10, 0x10, 0x10, 0x10, 0x7C, 0x00, 0x00 },
	[0x00F0] { 0x00, 0x24, 0x18, 0x28, 0x04, 0x3C, 0x42, 0x42, 0x42, 0x42, 0x3C, 0x00, 0x00 },
	[0x00F1] { 0x00, 0x00, 0x32, 0x4C, 0x00, 0x5C, 0x62, 0x42, 0x42, 0x42, 0x42, 0x00, 0x00 },
	[0x00F2] { 0x00, 0x00, 0x20, 0x10, 0x00, 0x3C, 0x42, 0x42, 0x42, 0x42, 0x3C, 0x00, 0x00 },
	[0x00F3] { 0x00, 0x00, 0x08, 0x10, 0x00, 0x3C, 0x42, 0x42, 0x42, 0x42, 0x3C, 0x00, 0x00 },
	[0x00F4] { 0x00, 0x00, 0x18, 0x24, 0x00, 0x3C, 0x42, 0x42, 0x42, 0x42, 0x3C, 0x00, 0x00 },
	[0x00F5] { 0x00, 0x00, 0x32, 0x4C, 0x00, 0x3C, 0x42, 0x42, 0x42, 0x42, 0x3C, 0x00, 0x00 },
	[0x00F6] { 0x00, 0x00, 0x24, 0x24, 0x00, 0x3C, 0x42, 0x42, 0x42, 0x42, 0x3C, 0x00, 0x00 },
	[0x00F7] { 0x00, 0x00, 0x00, 0x10, 0x10, 0x00, 0x7C, 0x00, 0x10, 0x10, 0x00, 0x00, 0x00 },
	[0x00F8] { 0x00, 0x00, 0x00, 0x00, 0x02, 0x3C, 0x46, 0x4A, 0x52, 0x62, 0x3C, 0x40, 0x00 },
	[0x00F9] { 0x00, 0x00, 0x20, 0x10, 0x00, 0x44, 0x44, 0x44, 0x44, 0x44, 0x3A, 0x00, 0x00 },
	[0x00FA] { 0x00, 0x00, 0x08, 0x10, 0x00, 0x44, 0x44, 0x44, 0x44, 0x44, 0x3A, 0x00, 0x00 },
	[0x00FB] { 0x00, 0x00, 0x18, 0x24, 0x00, 0x44, 0x44, 0x44, 0x44, 0x44, 0x3A, 0x00, 0x00 },
	[0x00FC] { 0x00, 0x00, 0x28, 0x28, 0x00, 0x44, 0x44, 0x44, 0x44, 0x44, 0x3A, 0x00, 0x00 },
	[0x00FD] { 0x00, 0x00, 0x08, 0x10, 0x00, 0x42, 0x42, 0x42, 0x46, 0x3A, 0x02, 0x42, 0x3C },
	[0x00FE] { 0x00, 0x00, 0x00, 0x40, 0x40, 0x5C, 0x62, 0x42, 0x42, 0x62, 0x5C, 0x40, 0x40 },
	[0x00FF] { 0x00, 0x00, 0x24, 0x24, 0x00, 0x42, 0x42, 0x42, 0x46, 0x3A, 0x02, 0x42, 0x3C },
};


const uint8_t *Graphics_GetFont(unsigned char c)
{
	return font_8x13[(c & 0xff)];  // only have 256 entries in the font
}


int Graphics_FontWidth(void)
{
	return FONT_WIDTH;
}


int Graphics_FontHeight(void)
{
	return FONT_HEIGHT;
}