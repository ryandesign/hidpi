/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#include "embiggen.h"

#include "constants.h"
#include "globals.h"

void embiggen_cursor(void)
{
	BitMap src_bits, dst_bits;

	src_bits.rowBytes = k_cursor_rowbytes;
	dst_bits.rowBytes = k_cursor_rowbytes_2x;

	src_bits.baseAddr = (Ptr)&TheCrsr.data;
	dst_bits.baseAddr = (Ptr)g_data->data_2x;

	SetRect(&src_bits.bounds, 0, 0, k_cursor_width, 2 * k_cursor_height);
	SetRect(&dst_bits.bounds, 0, 0, k_cursor_width_2x, 2 * k_cursor_height_2x);

	CopyBits(&src_bits, &dst_bits, &src_bits.bounds, &dst_bits.bounds, srcCopy, nil);
}

void embiggen_point(Point *point)
{
	point->v *= k_scale;
	point->h *= k_scale;
}

void embiggen_rect(Rect *rect)
{
	rect->top *= k_scale;
	rect->left *= k_scale;
	rect->bottom *= k_scale;
	rect->right *= k_scale;
}
