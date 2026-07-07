/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#include "JHideCursor_patch.h"

#include "constants.h"
#include "globals.h"
#include "macros.h"

static pascal void JHideCursor_2x(void);

pascal void JHideCursor_patch(void)
{
	declare(old);

	CrsrBusy = true;

	require(CrsrVis, CrsrVis);

	save_regs();
	JHideCursor_2x();
	restore_regs();

CrsrVis:
	--CrsrState;
	CrsrBusy = false;
}

static pascal void JHideCursor_2x(void)
{
	register short y;

	y = rect_height(&CrsrRect);

	if (y > 0)
	{
		register long *save;
		register long *screen;
		register long screen_row_advance;

		{
			long token;

			begin_globals(&token);
			save = g_data->save_2x;
			screen = (long *)CrsrAddr;
			end_globals(token);
		}

		screen_row_advance = (ScreenRow >> 2) - (k_cursor_save_rowlongs_2x - 1);

		do
		{
			*screen++ = *save++;
			*screen = *save++;
#if k_cursor_rowlongs_2x > 2
			*screen = *save++;
#endif
			screen += screen_row_advance;
		}
		while (--y);
	}

	CrsrVis = false;
}
