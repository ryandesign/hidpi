/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#include "JSetCrsr_patch.h"

#include "cursor_stuff.h"
#include "globals.h"
#include "JHideCursor_patch.h"
#include "JShowCursor_patch.h"
#include "macros.h"

static void JSetCrsr_2x(Point hotspot, short height, Ptr data, Ptr mask);

pascal void JSetCrsr_patch(Point hotspot, short height, Ptr data, Ptr mask)
{
	long token;

	declare(old);

	save_regs();
	begin_globals(&token);
	JSetCrsr_2x(hotspot, height, data, mask);
	end_globals(token);
	restore_regs();
}

static void JSetCrsr_2x(Point hotspot, short height, Ptr data, Ptr mask)
{
	register Boolean hotspot_changed;
	register Boolean bits_changed = false;

	// Copy the incoming 1x cursor data and mask into TheCrsr while keeping
	// track of whether this changed the bits. All cursors have the same size
	// bitmap and the mask always follows the data so the height and mask
	// parameters are ignored.
	{
		register long *src;
		register long *dst;
		register long chunk;
		register short num_chunks;

		src = (long *)data;
		dst = (long *)&TheCrsr.data;
		num_chunks = 2 * k_cursor_bytes / sizeof chunk;
		do
		{
			chunk = *src++;
			if (*dst != chunk)
			{
				bits_changed = true;
			}
			*dst++ = chunk;
		}
		while (--num_chunks);
	}

	pin_hotspot(&hotspot);

	// Update hotspot if it changed.
	hotspot_changed = *(long *)&hotspot != *(long *)&TheCrsr.hotSpot;
	if (hotspot_changed)
	{
		TheCrsr.hotSpot = hotspot;
		if (!bits_changed)
		{
			sync_hotspot_2x();
			JHideCursor_patch();
			JShowCursor_patch();
		}
	}

	if (bits_changed)
	{
		g_data->cursor_changed = true;
	}

#if 0
	if (bits_changed)
	{
		SetUpA5();

		if (false) // if found 2x asset
		{
			// load 2x asset
			// except we can't do that at iterrupt time
			// we're going to have to set a flag and patch systemtask
			// to check if the flag is set and do the heavy lifting then.
		}
		else
		{
			pixdub((Ptr)&TheCrsr.data,
				(Ptr)g_data->data,
				k_cursor_width / k_pixels_per_byte,
				k_cursor_width_2x / k_pixels_per_byte,
				2 * k_cursor_height);

/*
// Can't call SetRect or CopyBits at interrupt time.
// SetCursor can be called at interrupt time.
			BitMap src_bits, dst_bits;

			SetRect(&src_bits.bounds, 0, 0, k_cursor_width, 2 * k_cursor_height);
			src_bits.rowBytes = k_cursor_rowbytes;
			src_bits.baseAddr = (Ptr)&TheCrsr.data;

			SetRect(&dst_bits.bounds, 0, 0, k_cursor_width_2x, 2 * k_cursor_height_2x);
			dst_bits.rowBytes = k_cursor_rowbytes_2x;
			dst_bits.baseAddr = (Ptr)g_data->data;

			CopyBits(&src_bits, &dst_bits, &src_bits.bounds, &dst_bits.bounds, srcCopy, nil);
*/
		}

		RestoreA5();
	}

	// If anything changed, recalculate CrsrRect and CrsrAddr and update onscreen display.
	if (bits_changed || hotspot_changed)
	{
		JHideCursor_patch();
		JShowCursor_patch();
	}
#endif
}
