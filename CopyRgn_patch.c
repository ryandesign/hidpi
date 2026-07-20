/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#include "CopyRgn_patch.h"

#include "constants.h"
#include "globals.h"
#include "macros.h"
#include "rgnset.h"

typedef struct
{
	long saved_registers[1]; // A6
	long return_address;
	RgnHandle dst;
	RgnHandle src;
}
CopyRgn_stack;

static void CopyRgn_big(RgnHandle src, RgnHandle dst);

pascal void CopyRgn_patch(void)
{
	asm
	{
		bsr 	@start							; Push token address; skip placeholders.
@token	dc.l	0								; Placeholder for globals token.
@orig	dc.l	k_placeholder					; Placeholder for old routine address.

@start	jsr		begin_globals					; Activate globals.
		addq.l	#4, sp							; Pop token address.

		link	a6, #0							; Create stack frame.

		move.l	CopyRgn_stack.dst(a6), -(sp)	; Push destination region.
		move.l	CopyRgn_stack.src(a6), -(sp)	; Push source region.
		jsr		CopyRgn_big						; Call our routine.

		move.l	@token, -(sp)					; Push token.
		jsr		end_globals						; Deactivate globals.

		unlk	a6								; Delete stack frame.

extern CopyRgn_orig:
		move.l	@orig, -(sp)					; Push old routine address.
		return									; "Return" to old routine.
	}
}

static void CopyRgn_big(RgnHandle src, RgnHandle dst)
{
	rgnset_h srcrh, dstrh;
	rgnset_p srcrp, dstrp;

	srcrh = get_rgnset(src);
	require(srcrh, get_rgnset_src);

	dstrh = get_rgnset(dst);
	require(dstrh, get_rgnset_dst);

	srcrp = *srcrh;
	dstrp = *dstrh;

	CopyRgn_orig(srcrp->copy, dstrp->copy);
	CopyRgn_orig(srcrp->big, dstrp->big);

get_rgnset_dst:
get_rgnset_src:
	;
}
