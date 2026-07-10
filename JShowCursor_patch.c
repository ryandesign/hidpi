/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#include "JShowCursor_patch.h"

#include "constants.h"
#include "cursor_stuff.h"
#include "debigulate.h"
#include "globals.h"
#include "macros.h"

static void JShowCursor_2x(void);

pascal void JShowCursor_patch(void)
{
	long token;

	declare(old);

	CrsrBusy = true;

	save_regs();
	begin_globals(&token);

	++CrsrState;
	debigulate();
	require(CrsrState >= 0, CrsrState);

	CrsrState = 0;

	nrequire(CrsrVis, CrsrVis);

	JShowCursor_2x();

	CrsrVis = true;

CrsrVis:
CrsrState:
	end_globals(token);
	restore_regs();

	CrsrBusy = false;
}

// TODO: Review register use in this function.
static void JShowCursor_2x(void)
{
	register long *cursor_data;
	register long *cursor_mask;
	register short height, offset;
	short left, top, which = 0;

	cursor_data = g_data->data_2x;
	cursor_mask = g_data->mask_2x;

	// Compute the big cursor rect.
	{
		Rect screen_rect;

		get_screen_rect_big(&screen_rect);

		left = k_scale * Mouse.h - g_data->hotspot_2x.h;
		offset = left & (k_cursor_width_2x - 1);
		left -= offset;

		// Is the cursor partially off the left of the screen?
		{
			register short min_left = screen_rect.left;
	
			if (left < min_left)
			{
				--which;
				left = min_left;
			}
		}

		// Is the cursor partially off the right of the screen?
		{
			register short max_left = screen_rect.right - k_cursor_rect_width_2x;
	
			if (left > max_left)
			{
				++which;
				left = max_left;
			}
		}

		height = k_cursor_height_2x;
		top = k_scale * Mouse.v - g_data->hotspot_2x.v;

		// Is the cursor partially off the top of the screen?
		{
			register short min_top = screen_rect.top;

			if (top < min_top)
			{
				top = min_top + top;
				height += top;
				top *= k_cursor_rowlongs_2x;
				cursor_data -= top;
				cursor_mask -= top;
				top = min_top;
			}
		}

		// Is the cursor partially off the bottom of the screen?
		{
			register short max_bottom = screen_rect.bottom;
	
			if (top + k_cursor_height_2x > max_bottom)
			{
				height = max_bottom - top;
			}
		}
	}

	// Save the new big cursor rect.
	CrsrRect.top = top;
	CrsrRect.left = left;
	CrsrRect.bottom = top + height;
	CrsrRect.right = left + k_cursor_rect_width_2x;

	// Draw the cursor.
	if (height > 0)
	{
		long screen_row_advance;
		register long *screen;
		register long *save = g_data->save_2x;
		register long chunk, data_chunk, mask_chunk;
		long left_mask = (unsigned long)0xFFFFFFFF >> offset;
		// TODO: we're short on registers; maybe use a single mask variable
		// and negate it twice every time through the loop?
		long right_mask = ~left_mask;

		// TODO: This won't be right for 3x/4x
		if (which < 0)
		{
			// Partially off the left of the screen: Draw only right part of cursor.
			left_mask = right_mask;
			right_mask = 0;
		}
		if (which > 0)
		{
			// Partially off the right of the screen: Draw only left part of cursor.
			right_mask = left_mask;
			left_mask = 0;
		}

		screen_row_advance = ScreenRow;
		screen = (long *)((Ptr)ScrnBase + top * screen_row_advance + left / k_pixels_per_byte);
		screen_row_advance >>= 2; // /= sizeof(long)
		CrsrAddr = (Ptr)screen;
		screen_row_advance -= k_cursor_save_rowlongs_2x - 1;

		// TODO: This is very 2x specific
		do
		{
			// Grab a chunk from the screen and save it.
			chunk = *screen;
			*save++ = chunk;

			// Load cursor image chunk and align.
			data_chunk = *cursor_data++;
			ror(offset, data_chunk);
			
			// Load cursor mask chunk and align.
			mask_chunk = *cursor_mask++;
			ror(offset, mask_chunk);

			// Superimpose the left cursor chunk.
			chunk &= ~(mask_chunk & left_mask);
			chunk ^= data_chunk & left_mask;

			// Put it onscreen.
			*screen++ = chunk;

// TODO: 3x and 4x are untested
#if k_scale >= 3
			// Grab a chunk from the screen and save it.
			chunk = *screen;
			*save++ = chunk;

			// Superimpose the right cursor chunk.
			chunk &= ~(mask_chunk & right_mask);
			chunk ^= data_chunk & right_mask;

			// Load cursor image chunk and align.
			data_chunk = *cursor_data++;
			ror(offset, data_chunk);
			
			// Load cursor mask chunk and align.
			mask_chunk = *cursor_mask++;
			ror(offset, mask_chunk);

			// Superimpose the left cursor chunk.
			chunk &= ~(mask_chunk & left_mask);
			chunk ^= data_chunk & left_mask;

			// Put it onscreen.
			*screen++ = chunk;
#endif

			// Grab a chunk from the screen and save it.
			chunk = *screen;
			*save++ = chunk;

			// Superimpose the right cursor chunk.
			chunk &= ~(mask_chunk & right_mask);
			chunk ^= data_chunk & right_mask;

			// Put it onscreen.
			*screen = chunk;
			screen += screen_row_advance;
		}
		while (--height);
	}
}
