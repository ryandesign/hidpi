/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#include "JInitCrsr_patch.h"

#include "globals.h"
#include "JShowCursor_patch.h"
#include "macros.h"

pascal void JInitCrsr_patch(void)
{
	declare(old);

	CrsrObscure = 0;
	CrsrState = 0;

	// Jump to JShowCursor_patch.
	jmp(JShowCursor_patch);
	//JShowCursor_patch();
}
