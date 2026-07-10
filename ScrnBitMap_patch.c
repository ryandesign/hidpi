/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#include "ScrnBitMap_patch.h"

#include "constants.h"
#include "globals.h"
#include "macros.h"

pascal void ScrnBitMap_patch(BitMap *bitmap)
{
	register ScrnBitMap_proc_ptr old_ScrnBitMap;

	declare(old);

	movea(old, old_ScrnBitMap);
	old_ScrnBitMap(bitmap);
	*(long *)&topLeft(bitmap->bounds) /= k_scale;
	*(long *)&botRight(bitmap->bounds) /= k_scale;
}

void ScrnBitMap_big(BitMap *bitmap)
{
	ScrnBitMap_patch(bitmap);
	*(long *)&topLeft(bitmap->bounds) *= k_scale;
	*(long *)&botRight(bitmap->bounds) *= k_scale;
}
