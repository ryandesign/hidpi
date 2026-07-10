/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#include "cursor_stuff.h"

#include "constants.h"
#include "embiggen.h"
#include "globals.h"
#include "JHideCursor_patch.h"
#include "JShowCursor_patch.h"
#include "ScrnBitMap_patch.h"

void get_screen_rect_big(Rect *rect)
{
	BitMap bitmap;

	// TODO: Color QuickDraw
	ScrnBitMap_big(&bitmap);
	*rect = bitmap.bounds;
}

void pin_hotspot(Point *hotspot)
{
	register short max;

	// Pin hotspot x to valid range.
	max = k_cursor_width;
	if ((unsigned short)hotspot->h > max)
	{
		hotspot->h = max;
	}

	// Pin hotspot y to valid range.
#if k_cursor_width != k_cursor_height
	max = k_cursor_height;
#endif
	if ((unsigned short)hotspot->v > max)
	{
		hotspot->v = max;
	}
}

void sync_hotspot_2x(void)
{
	*(long *)&g_data->hotspot_2x = *(long *)&TheCrsr.hotSpot * k_scale;
}

void load_cursor_2x(void)
{
	//long final_ticks;
	//Delay(60, &final_ticks);

	if (false) // if found 2x asset
	{
		// load 2x asset
		// except we can't do that at interrupt time
		// we're going to have to set a flag and patch systemtask
		// to check if the flag is set and do the heavy lifting then.
	}
	else
	{
		embiggen_cursor();
	}

	sync_hotspot_2x();
	JHideCursor_patch();
	JShowCursor_patch();
}
