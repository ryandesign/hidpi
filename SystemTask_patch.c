/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#include "SystemTask_patch.h"

#include "globals.h"
#include "macros.h"
#include "cursor_stuff.h"
#include "patch_table.h"

static pascal void SystemTask_2x(void);

pascal void SystemTask_patch(void)
{
	declare(old_SystemTask);

// TODO: do we really need to check CrsrBusy?
	nrequire(CrsrBusy, CrsrBusy);

	save_regs();
	SystemTask_2x();
	restore_regs();

CrsrBusy:
	// Jump to old SystemTask.
	push(old_SystemTask);
}

static pascal void SystemTask_2x(void)
{
	long token;

	begin_globals(&token);

	if (g_data->cursor_changed)
	{
		load_cursor_2x();
		g_data->cursor_changed = false;
	}

	end_globals(token);
}
