/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#include "CloseRgn_patch.h"

#include "globals.h"
#include "macros.h"
#include "rgnset.h"

typedef struct
{
	long saved_registers[1]; // A6
	long return_address;
	RgnHandle rgn;
}
CloseRgn_stack;

static void CloseRgn_big(RgnHandle rgn);

pascal void CloseRgn_patch(void)
{
	asm
	{
		bsr 	@start							; Push token address; skip placeholders.
@token	dc.l	0								; Placeholder for globals token.
@orig	dc.l	k_placeholder					; Placeholder for old routine address.

@start	jsr		begin_globals					; Activate globals.
		addq.l	#4, sp							; Pop token address.

		link	a6, #0							; Create stack frame.

		move.l	CloseRgn_stack.rgn(a6), -(sp)	; Push region.
		jsr		CloseRgn_big					; Call our routine.

		move.l	@token, -(sp)					; Push token.
		jsr		end_globals						; Deactivate globals.

		unlk	a6								; Delete stack frame.

extern CloseRgn_orig:
		move.l	@orig, -(sp)					; Push old routine address.
		return									; "Return" to old routine.
	}
}

static void CloseRgn_big(RgnHandle rgn)
{
//	long token;
	rgntmp_t tmp;
	rgnset_h rh;
	qd_globals_t *qd;
	GrafPtr thePort;
	long rgnSave;

	qd = get_qd_globals();
	thePort = qd->thePort;
	get_rgnSave(rgnSave, thePort);
	require(rgnSave, get_rgnSave);

	rh = get_rgnset(rgn);
	require(rh, get_rgnset);

//	begin_globals(&token);

	tmp = qd->rgntmp;
	qd->rgntmp = g_data->rgntmp_big;
	CloseRgn_orig((**rh).big);
	qd->rgntmp = tmp;

	g_data->closed_rgnset = rh;
	g_data->closed_rgnbuf = tmp.buf;

//	end_globals(token);

	set_rgnSave(rgnSave, thePort);

get_rgnset:
get_rgnSave:
	;
}
