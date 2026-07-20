/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#include "embiggen.h"

#include "constants.h"
#include "globals.h"

static short embiggened_short(short src_short)
{
	short src = src_short;

	return (src > k_qd_max_big) ? k_qd_max : (src < k_qd_min_big) ? k_qd_min : (k_scale * src);
}

static short embiggened_rgn_short(short src_short)
{
	short src = src_short;

	return (k_qd_rgn_flag == src) ? k_qd_rgn_flag : embiggened_short(src);
}

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
	point->v = embiggened_short(point->v);
	point->h = embiggened_short(point->h);
}

void embiggen_rect(Rect *rect)
{
	rect->top = embiggened_short(rect->top);
	rect->left = embiggened_short(rect->left);
	rect->bottom = embiggened_short(rect->bottom);
	rect->right = embiggened_short(rect->right);
}

void embiggen_rgn(RgnHandle src, RgnHandle dst)
{
	short *srcp, *dstp;
	short count;

	dstp = &((short *)*dst)[1];
	srcp = (short *)*src;
	count = *srcp++ / sizeof(short);
	while (--count)
	{
		*dstp++ = embiggened_rgn_short(*srcp++);
	}
}
