/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#include "uninstall.h"

#include <Traps.h>

#include "embiggen.h"
#include "macros.h"

void uninstall(patch_t *patches)
{
	patch_t *patch = patches;
	long *jump;
	short routine;

	while (0 != (routine = patch->routine))
	{
//		long old_address = patch->value.old_address;
		patch_proc_t *patch_proc = (patch_proc_t *)((Ptr)&patch->offset + patch->offset);
		long old_address = *(long *)&patch_proc->old_address[patch->old_address_index];
		short type = routine & 0xF000;

		if (type)
		{
			// It's a trap!
			if ((long)nil != old_address)
			{
				type = get_trap_type(routine);
				NSetTrapAddress(old_address, routine, type);
			}
		}
		else
		{
			// It's a low-memory jump location.
			jump = (long *)routine;
			*jump = old_address;
		}

		++patch;
	}

	// TODO: Find the more correct place to adjust CrsrPin.
	embiggen_rect(&CrsrPin);
}
