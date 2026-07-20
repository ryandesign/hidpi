/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#include "OffsetRgn_patch.h"

#include "globals.h"
#include "macros.h"
#include "rgnset.h"

typedef struct
{
	long saved_registers[1]; // A6
	long return_address;
	short dv;
	short dh;
	RgnHandle rgn;
}
OffsetRgn_stack;

static void OffsetRgn_big(RgnHandle rgn, short dh_in, short dv_in);

pascal void OffsetRgn_patch(void)
{
	asm
	{
		bsr 	@start							; Push token address; skip placeholders.
@token	dc.l	0								; Placeholder for globals token.
@orig	dc.l	k_placeholder					; Placeholder for old routine address.

@start	jsr		begin_globals					; Activate globals.
		addq.l	#4, sp							; Pop token address.

		link	a6, #0							; Create stack frame.

		move.w	OffsetRgn_stack.dv(a6), -(sp)	; Push dv.
		move.w	OffsetRgn_stack.dh(a6), -(sp)	; Push dh.
		move.l	OffsetRgn_stack.rgn(a6), -(sp)	; Push region.
		jsr		OffsetRgn_big					; Call our routine.

		move.l	@token, -(sp)					; Push token.
		jsr		end_globals						; Deactivate globals.

		unlk	a6								; Delete stack frame.

extern OffsetRgn_orig:
		move.l	@orig, -(sp)					; Push old routine address.
		return									; "Return" to old routine.
	}
}

static void OffsetRgn_big(RgnHandle rgn, short dh_in, short dv_in)
{
	rgnset_h rh;
	rgnset_p rp;
	short dh = dh_in;
	short dv = dv_in;

	rh = get_rgnset(rgn);
	require(rh, get_rgnset);
	rp = *rh;

	OffsetRgn_orig(rp->copy, dh, dv);
	OffsetRgn_orig(rp->big, k_scale * dh, k_scale * dv);

get_rgnset:
	;
}
