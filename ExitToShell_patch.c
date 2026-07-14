/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#include "ExitToShell_patch.h"

#include "globals.h"
#include "macros.h"
#include "patch_table.h"
#include "qdprocs.h"
#include "uninstall.h"

static void ExitToShell_big(void);

pascal void ExitToShell_patch(void)
{
	declare(old_ExitToShell);

	save_regs();
	ExitToShell_big();
	restore_regs();

	// Jump to old ExitToShell.
	push(old_ExitToShell);
}

static void ExitToShell_big(void)
{
	long token;

	begin_globals(&token);
#ifdef USE_TRAP_PATCHING
	if (g_installed) uninstall(get_patch_table());
#endif
	deinit_qdprocs();
	end_globals(token);
}
