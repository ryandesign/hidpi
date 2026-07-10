/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#include "ScrnBitMap_patch.h"

#include "constants.h"
#include "debigulate.h"
#include "embiggen.h"
#include "globals.h"
#include "macros.h"

pascal void ScrnBitMap_patch(BitMap *bitmap)
{
	register ScrnBitMap_proc_ptr old_ScrnBitMap;

	declare(old);

	save_regs();

	movea(old, old_ScrnBitMap);
	old_ScrnBitMap(bitmap);
	debigulate_rect(&bitmap->bounds);

	restore_regs();
}

void ScrnBitMap_big(BitMap *bitmap)
{
	// Surely this can be structured better. It's silly to debigulate
	// and then immediately rebigulate.
	ScrnBitMap_patch(bitmap);
	embiggen_rect(&bitmap->bounds);
}
