/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#include "JCrsrObscure_patch.h"

#include "globals.h"
#include "JHideCursor_patch.h"
#include "macros.h"

pascal void JCrsrObscure_patch(void)
{
	declare(old);

	CrsrBusy = true;

	if (!CrsrObscure)
	{
		CrsrObscure = true;

		// JHideCursor_patch sets CrsrBusy to false.
		jmp(JHideCursor_patch);
	}

	CrsrBusy = false;
}
