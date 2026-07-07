/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#include "JScrnSize_patch.h"

#include "constants.h"
#include "globals.h"
#include "macros.h"

pascal void JScrnSize_patch(short *width, short *height)
{
	register JScrnSize_proc_ptr old_JScrnSize;

	declare(old);

DebugStr("\pJScrnSize");
	movea(old, old_JScrnSize);
	old_JScrnSize(width, height);
	*width /= k_scale;
	*height /= k_scale;
}
