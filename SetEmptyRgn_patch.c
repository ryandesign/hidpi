/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#include "SetEmptyRgn_patch.h"

#include "globals.h"
#include "macros.h"
#include "rgnset.h"

typedef struct
{
	long saved_registers[1]; // A6
	long return_address;
	RgnHandle rgn;
}
SetEmptyRgn_stack;

static void SetEmptyRgn_big(RgnHandle rgn);

pascal void SetEmptyRgn_patch(void)
{
	asm
	{
		bsr 	@start							; Push token address; skip placeholders.
@token	dc.l	0								; Placeholder for globals token.
@orig	dc.l	k_placeholder					; Placeholder for old routine address.

@start	jsr		begin_globals					; Activate globals.
		addq.l	#4, sp							; Pop token address.

		link	a6, #0							; Create stack frame.

		move.l	SetEmptyRgn_stack.rgn(a6), -(sp); Push region.
		jsr		SetEmptyRgn_big					; Call our routine.

		move.l	@token, -(sp)					; Push token.
		jsr		end_globals						; Deactivate globals.

		unlk	a6								; Delete stack frame.

extern SetEmptyRgn_orig:
		move.l	@orig, -(sp)					; Push old routine address.
		return									; "Return" to old routine.
	}
}

static void SetEmptyRgn_big(RgnHandle rgn)
{
	rgnset_h rh;
	rgnset_p rp;

	rh = get_rgnset(rgn);
	require(rh, get_rgnset);
	rp = *rh;

	SetEmptyRgn_orig(rp->copy);
	SetEmptyRgn_orig(rp->big);

get_rgnset:
	;
}
