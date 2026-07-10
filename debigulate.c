/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#include "debigulate.h"

#include <stdarg.h>

#include "constants.h"
#include "globals.h"
#include "macros.h"
#include "missing_traps.h"

void debigulate_later(short first_item_type, ...)
{
	va_list args;
	debigulation_t *debigulation;
	short count = 0;
	short item_type = first_item_type;

	va_start(args, first_item_type);

	while (0 != item_type)
	{
		if (count >= k_max_debigulations)
		{
			DebugStr("\ptoo many debigulations");
			--count;
			break;
		}

		debigulation = &g_data->debigulations[count++];

		debigulation->type = item_type;
		debigulation->item = va_arg(args, void *);
		debigulation->orig_item = va_arg(args, void *);

		item_type = va_arg(args, short);
	}

	g_data->num_debigulations = count;
	va_end(args);
}

void debigulate(void)
{
	short count;

	require(CrsrState == g_data->debigulate_crsrstate, CrsrState);

	count = g_data->num_debigulations;
	require(count > 0, count);

	do
	{
		debigulation_t *debigulation = &g_data->debigulations[--count];
		switch (debigulation->type)
		{
			case t_none:
				break;
			case t_PointPtr:
				if (nil != debigulation->orig_item)
				{
					*(Point *)debigulation->item = *(Point *)debigulation->orig_item;
				}
				else
				{
					debigulate_point((Point *)debigulation->item);
				}
				break;
			case t_RectPtr:
				if (nil != debigulation->orig_item)
				{
					*(Rect *)debigulation->item = *(Rect *)debigulation->orig_item;
				}
				else
				{
					debigulate_rect((Rect *)debigulation->item);
				}
				break;
			case t_RgnHandle:
				if (nil != debigulation->orig_item)
				{
					RgnPtr src = *(RgnHandle)debigulation->orig_item;

					BlockMoveData(src, *(RgnHandle)debigulation->item, src->rgnSize);
				}
				else
				{
					debigulate_rgn((RgnHandle)debigulation->item);
				}
				break;
		}
	}
	while (count > 0);

	g_data->num_debigulations = count;

count:
CrsrState:
	;
}

void debigulate_point(Point *point)
{
	point->v /= k_scale;
	point->h /= k_scale;
}

void debigulate_rect(Rect *rect)
{
	rect->top /= k_scale;
	rect->left /= k_scale;
	rect->bottom /= k_scale;
	rect->right /= k_scale;
}

void debigulate_rgn(RgnHandle rgn)
{
	RgnPtr rgnp = *rgn;

	debigulate_shorts((short *)&rgnp->rgnBBox, rgnp->rgnSize - sizeof rgnp->rgnSize);
}

void debigulate_shorts(short *buf, short size)
{
	short value;
	short count = size;
	short *p = buf;

	while (count > 0)
	{
		value = *p;
		if (32767 != value)
		{
			value /= k_scale;
		}
		*p++ = value;
		count -= 2;
	}
}
