/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#include "SectRgn_patch.h"

#include "globals.h"
#include "macros.h"
#include "rgnset.h"

typedef struct
{
	long saved_registers[1]; // A6
	long return_address;
	RgnHandle dst;
	RgnHandle src2;
	RgnHandle src1;
}
SectRgn_stack;

static void SectRgn_big(RgnHandle src1, RgnHandle src2, RgnHandle dst);

pascal void SectRgn_patch(void)
{
	asm
	{
		bsr 	@start							; Push token address; skip placeholders.
@token	dc.l	0								; Placeholder for globals token.
@orig	dc.l	k_placeholder					; Placeholder for old routine address.

@start	jsr		begin_globals					; Activate globals.
		addq.l	#4, sp							; Pop token address.

		link	a6, #0							; Create stack frame.

		move.l	SectRgn_stack.dst(a6), -(sp)	; Push destination region.
		move.l	SectRgn_stack.src2(a6), -(sp)	; Push source region 2.
		move.l	SectRgn_stack.src1(a6), -(sp)	; Push source region 1.
		jsr		SectRgn_big						; Call our routine.

		move.l	@token, -(sp)					; Push token.
		jsr		end_globals						; Deactivate globals.

		unlk	a6								; Delete stack frame.

extern SectRgn_orig:
		move.l	@orig, -(sp)					; Push old routine address.
		return									; "Return" to old routine.
	}
}

static void SectRgn_big(RgnHandle src1, RgnHandle src2, RgnHandle dst)
{
	rgnset_h src1rh, src2rh, dstrh;
	rgnset_p src1rp, src2rp, dstrp;

	src1rh = get_rgnset(src1);
	require(src1rh, get_rgnset_src1);

	src2rh = get_rgnset(src2);
	require(src2rh, get_rgnset_src2);

	dstrh = get_rgnset(dst);
	require(dstrh, get_rgnset_dst);

	src1rp = *src1rh;
	src2rp = *src2rh;
	dstrp = *dstrh;

	SectRgn_orig(src1rp->copy, src2rp->copy, dstrp->copy);
	SectRgn_orig(src1rp->big, src2rp->big, dstrp->big);

get_rgnset_dst:
get_rgnset_src2:
get_rgnset_src1:
	;
}
