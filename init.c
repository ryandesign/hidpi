/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#include <Traps.h>

#include "globals.h"
#include "init_data.h"
#include "install.h"
#include "macros.h"
#include "system_requirements.h"

#define r_patch_type 'PTCH'
#define r_patch 0

#define r_cicn_good 128
#define r_cicn_bad 129

#define k_shift_key_bit 0x1
#define k_1_key_bit 0x400
#define k_2_key_bit 0x800

void main(void)
{
	KeyMap keymap;
	code_t **code = nil;
	Size sizeof_code;
	code_t *combined;
	data_t *data;
	globals_t *globals;
	patch_t *patches;
	short icon = r_cicn_bad;

	// Ensure the mouse button isn't being held down.
	nrequire(Button(), Button);

	// Ensure the Shift key isn't being held down.
	GetKeys(keymap);
	nrequire(keymap[1] & k_shift_key_bit, GetKeys);

	// Ensure system requirements are met.
	require(system_requirements_met(), system_requirements_met);

	// During development, require the user to press 1 at startup to disable
	// the INIT or 2 to enable it. If they don't within 2 seconds, disable it.
	// TODO: Remove.
	{
		long end_ticks = TickCount() + 2 * 60;

		do
		{
			GetKeys(keymap);
		}
		while (0 == (keymap[0] & (k_1_key_bit | k_2_key_bit)) && TickCount() < end_ticks);

		// Ensure the 2 key is being held down.
		require(keymap[0] & k_2_key_bit, GetKeys);
	}

	// Get the resource containing the patch code.
	code = (code_t **)Get1Resource(r_patch_type, r_patch);
	require(code, Get1Resource);

	// Move the code out of the way and lock it.
	HLockHi((Handle)code);
	nrequire(MemError(), HLockHi);

	// Get the size of the code.
	sizeof_code = GetHandleSize((Handle)code);
	require(sizeof_code >= sizeof globals, GetHandleSize);

	// Allocate a block of memory the size of the code and data.
	combined = (code_t *)NewPtrSysClear(sizeof_code + sizeof *data);
	require(combined, NewPtrSysClear);

	// Copy the code into the combined block.
	BlockMove(*code, combined, sizeof_code);

	// References to globals are computed from the start of the code.
	combined->gp = (Ptr)combined;

	// Data will be after the code.
	data = (data_t *)((Ptr)combined + sizeof_code);
	// TODO: Align the data to start at a multiple of 4, possibly by
	// moving the data in front of the code. (Blocks from NewPtr and
	// NewHandle are already aligned.)

	// Insider info: THINK C puts globals at the end of the code resource.
	globals = (globals_t *)((Ptr)data - sizeof *globals);

	// Locate data global.
	require((data_t *)k_uninitialized_data == globals->data, got_data_global);
	globals->data = data;

	// Initialize data.
	require(init_data(data), init_data);

	// Get address of patch table.
	patches = combined->patches;

	// Install the patches and fill in the old routine addresses.
	require(install(patches), install);
	// TODO: Separate preflight from install. Make preflight return the
	// size of the patch table and don't include it when BlockMoving the
	// code. Requires moving the globals pointer after the patch table.

	// Done!
	icon = r_cicn_good;
	goto done;

install:
init_data:
got_data_global:
	DisposePtr((Ptr)combined);
NewPtrSysClear:
GetHandleSize:
HLockHi:
Get1Resource:
system_requirements_met:
GetKeys:
Button:

done:
	if (nil != code)
	{
		ReleaseResource((Handle)code);
	}

	// TODO: showicon
}
