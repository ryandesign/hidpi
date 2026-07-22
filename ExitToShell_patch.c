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
	asm
	{
		bsr.s 	@start							; Push token address; skip placeholders.
@token	dc.l	0								; Placeholder for globals token.
@orig	dc.l	k_placeholder					; Placeholder for old routine address.

@start	jsr		begin_globals					; Activate globals.

		jsr		ExitToShell_big					; Call our routine.

		move.l	@token, (sp)					; Pop token address; push token.
		jsr		end_globals						; Deactivate globals.
		addq.l	#4, sp							; Pop token.

extern ExitToShell_orig:
		move.l	@orig, -(sp)					; Push old routine address.
		return									; "Return" to old routine.
	}
}

static void ExitToShell_big(void)
{
#ifdef USE_TRAP_PATCHING
	if (g_installed) uninstall(get_patch_table());
#endif
	deinit_qdprocs();
}
