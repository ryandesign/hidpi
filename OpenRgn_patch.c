/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#include "OpenRgn_patch.h"

#include "globals.h"
#include "macros.h"
#include "rgnset.h"

typedef struct
{
	long return_address;
}
OpenRgn_stack;

static void OpenRgn_big(void);

pascal void OpenRgn_patch(void)
{
	asm
	{
		bsr 	@start							; Push token address; skip placeholders.
@token	dc.l	0								; Placeholder for globals token.
@orig	dc.l	k_placeholder					; Placeholder for old routine address.

@start	jsr		begin_globals					; Activate globals.

		jsr		OpenRgn_big						; Call our routine.

		move.l	@token, (sp)					; Pop token address; push token.
		jsr		end_globals						; Deactivate globals.
		addq.l	#4, sp							; Pop token.

extern OpenRgn_orig:
		move.l	@orig, -(sp)					; Push old routine address.
		return									; "Return" to old routine.
	}
}

static void OpenRgn_big(void)
{
//	long token;
	qd_globals_t *qd;
	GrafPtr thePort;
	long rgnSave;

	qd = get_qd_globals();
	thePort = qd->thePort;
	get_rgnSave(rgnSave, thePort);
	nrequire(rgnSave, get_rgnSave);

//	begin_globals(&token);

	OpenRgn_orig();
	g_data->rgntmp_big = qd->rgntmp;

//	end_globals(token);

	set_rgnSave(rgnSave, thePort);

get_rgnSave:
	;
}
