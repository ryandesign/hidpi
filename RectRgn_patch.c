/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#include "RectRgn_patch.h"

#include "embiggen.h"
#include "globals.h"
#include "macros.h"
#include "rgnset.h"

typedef struct
{
	long return_address;
	Rect *rect;
	RgnHandle rgn;
}
RectRgn_stack;

static void RectRgn_big(RectRgn_stack *stack);

pascal void RectRgn_patch(void)
{
	asm
	{
		bsr 	@start							; Push token address; skip placeholders.
@token	dc.l	0								; Placeholder for globals token.
@orig	dc.l	k_placeholder					; Placeholder for old routine address.

@start	jsr		begin_globals					; Activate globals.
		addq.l	#4, sp							; Pop token address.

		move.l	sp, -(sp)						; Push stack pointer.
		jsr		RectRgn_big						; Call our routine.
		addq.l	#4, sp							; Pop stack pointer.

		move.l	@token, -(sp)					; Push token.
		jsr		end_globals						; Deactivate globals.
		addq.l	#4, sp							; Pop token.

extern RectRgn_orig:
		move.l	@orig, -(sp)					; Push old routine address.
		return									; "Return" to old routine.
	}
}

static void RectRgn_big(RectRgn_stack *stack)
{
	rgnset_h rh;
	Rect rect;

	rh = get_rgnset(stack->rgn);
	require(rh, end);

	rect = *stack->rect;

	RectRgn_orig((**rh).copy, &rect);
	embiggen_rect(&rect);
	RectRgn_orig((**rh).big, &rect);

end:
	;
}
