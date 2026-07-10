/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#include "install.h"

#include <Traps.h>

#include "macros.h"

#define k_num_old_addresses 5

Boolean install(patch_t *patches)
{
	long unimplemented;
	patch_t *patch;
	patch_proc_t *patch_proc;
	long old;
	long *jump;
	short routine;
	short type;

	patch = patches;

	// Preflight check to ensure all routines are valid.
	while (0 != (routine = patch->routine))
	{
		short i;

		patch_proc = (patch_proc_t *)((Ptr)&patch->offset + patch->offset);
		for (i = 0; i < k_num_old_addresses; ++i)
		{
			if (0x4E714E71 == *(long *)&patch_proc->old_address[i])
			{
				patch->old_address_index = i;
				break;
			}
		}
		if (k_num_old_addresses == i)
		{
			DebugStr("\ppreflight failed");
			return false;
		}

		type = routine & 0xF000;
		switch (type)
		{
			case 0xA000:
			case 0x0000:
				break;
			default:
				return false;
		}

		++patch;
	}

	// Hide the 1x cursor.
	HideCursor();

	unimplemented = GetToolTrapAddress(_Unimplemented);
	patch = patches;

	while (0 != (routine = patch->routine))
	{
//		patch_value_t *value = &patch->value;
//		long patch_address = (long)&value->offset + value->offset.offset;
		patch_proc = (patch_proc_t *)((Ptr)&patch->offset + patch->offset);

		type = routine & 0xF000;
		if (type)
		{
			// It's a trap!
			type = get_trap_type(routine);
			old = NGetTrapAddress(routine, type);

			if (old != unimplemented)
			{
				NSetTrapAddress((long)patch_proc, routine, type);
			}
			else
			{
				old = (long)nil;
			}
		}
		else
		{
			// It's a low-memory jump location.
			jump = (long *)routine;
			old = *jump;
			*jump = (long)patch_proc;
		}

		*(long *)&patch_proc->old_address[patch->old_address_index] = old;
		++patch;
	}

	*(long *)&topLeft(CrsrPin) /= k_scale;
	*(long *)&botRight(CrsrPin) /= k_scale;

	return true;
}
