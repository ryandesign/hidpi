/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#include "DisposeRgn_patch.h"

#include "globals.h"
#include "rgnset.h"

typedef struct
{
	long return_address;
	RgnHandle rgn;
}
DisposeRgn_stack;

pascal void DisposeRgn_patch(void)
{
	asm
	{
		bsr.s 	@start							; Push token address; skip placeholders.
@token	dc.l	0								; Placeholder for globals token.
@orig	dc.l	k_placeholder					; Placeholder for old routine address.

@start	jsr		begin_globals					; Activate globals.
		addq.l	#4, sp							; Pop token address.

		move.l	DisposeRgn_stack.rgn(sp), -(sp)	; Push region.
		jsr		dispose_rgnset					; Call our routine.
		addq.l	#4, sp							; Pop region.

		move.l	@token, -(sp)					; Push token.
		jsr		end_globals						; Deactivate globals.
		addq.l	#4, sp							; Pop token.

extern DisposeRgn_orig:
		move.l	@orig, -(sp)					; Push old routine address.
		return									; "Return" to old routine.
	}
}
