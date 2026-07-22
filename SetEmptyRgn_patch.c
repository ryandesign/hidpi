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
	long return_address;
	RgnHandle rgn;
}
SetEmptyRgn_stack;

static void SetEmptyRgn_big(long return_address, RgnHandle rgn);

pascal void SetEmptyRgn_patch(void)
{
	asm
	{
		bsr.s	@start							; Push token address; skip placeholders.
@token	dc.l	0								; Placeholder for globals token.
@orig	dc.l	k_placeholder					; Placeholder for old routine address.

@start	jsr		begin_globals					; Activate globals.
		addq.l	#4, sp							; Pop token address.

		jsr		SetEmptyRgn_big					; Call our routine.

		move.l	@token, -(sp)					; Push token.
		jsr		end_globals						; Deactivate globals.
		addq.l	#4, sp							; Pop token.

extern SetEmptyRgn_orig:
		move.l	@orig, -(sp)					; Push old routine address.
		return									; "Return" to old routine.
	}
}

static void SetEmptyRgn_big(long return_address, RgnHandle rgn)
{
	rgnset_h rh;

	rh = get_rgnset(rgn);
	require(rh, end);

	SetEmptyRgn_orig((**rh).copy);
	SetEmptyRgn_orig((**rh).big);

end:
	;
}
