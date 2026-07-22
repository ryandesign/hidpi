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
	long return_address;
	RgnHandle dst;
	RgnHandle src;
}
CopyRgn_stack;

static void CopyRgn_big(CopyRgn_stack *stack);

pascal void CopyRgn_patch(void)
{
	asm
	{
		bsr.s 	@start							; Push token address; skip placeholders.
@token	dc.l	0								; Placeholder for globals token.
@orig	dc.l	k_placeholder					; Placeholder for old routine address.

@start	jsr		begin_globals					; Activate globals.
		addq.l	#4, sp							; Pop token address.

		move.l	sp, -(sp)						; Push stack pointer.
		jsr		CopyRgn_big						; Call our routine.
		addq.l	#4, sp							; Pop stack pointer.

		move.l	@token, -(sp)					; Push token.
		jsr		end_globals						; Deactivate globals.
		addq.l	#4, sp							; Pop token address.

extern CopyRgn_orig:
		move.l	@orig, -(sp)					; Push old routine address.
		return									; "Return" to old routine.
	}
}

static void CopyRgn_big(CopyRgn_stack *stack)
{
	rgnset_h srcrh, dstrh;

	srcrh = get_rgnset(stack->src);
	require(srcrh, end);

	dstrh = get_rgnset(stack->dst);
	require(dstrh, end);

	CopyRgn_orig((**srcrh).copy, (**dstrh).copy);
	CopyRgn_orig((**srcrh).big, (**dstrh).big);

end:
	;
}
