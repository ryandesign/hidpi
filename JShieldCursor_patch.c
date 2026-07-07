/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#include "JShieldCursor_patch.h"

#include "globals.h"
#include "JHideCursor_patch.h"
#include "macros.h"

static pascal void JShieldCursor_2x(short left, short top, short right, short bottom);

pascal void JShieldCursor_patch(short left, short top, short right, short bottom)
{
	declare(old);

	save_regs();
	JShieldCursor_2x(left, top, right, bottom);
	restore_regs();
}

static pascal void JShieldCursor_2x(short left, short top, short right, short bottom)
{
	register Rect *cursor_rect;

	cursor_rect = &CrsrRect;

	if (bottom <= cursor_rect->top
			|| right <= cursor_rect->left
			|| top >= cursor_rect->bottom
			|| left >= cursor_rect->right)
	{
		// Rects don't intersect.
		--CrsrState;
	}
	else
	{
		// Rects intersect.
		JHideCursor_patch();
	}
}
