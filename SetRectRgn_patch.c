/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#include "SetRectRgn_patch.h"

#include "embiggen.h"
#include "globals.h"
#include "macros.h"
#include "rgnset.h"

typedef struct
{
	long return_address;
	Point bottom_right;
	Point top_left;
	RgnHandle rgn;
}
SetRectRgn_stack;

static void SetRectRgn_big(SetRectRgn_stack *stack);

pascal void SetRectRgn_patch(void)
{
	asm
	{
		bsr.s 	@start							; Push token address; skip placeholders.
@token	dc.l	0								; Placeholder for globals token.
@orig	dc.l	k_placeholder					; Placeholder for old routine address.

@start	jsr		begin_globals					; Activate globals.
		addq.l	#4, sp							; Pop token address.

		move.l	sp, -(sp)						; Push stack pointer.
		jsr		SetRectRgn_big					; Call our routine.
		addq.l	#4, sp							; Pop stack pointer.

		move.l	@token, -(sp)					; Push token.
		jsr		end_globals						; Deactivate globals.
		addq.l	#4, sp							; Pop token.

extern SetRectRgn_orig:
		move.l	@orig, -(sp)					; Push old routine address.
		return									; "Return" to old routine.
	}
}

static void SetRectRgn_big(SetRectRgn_stack *stack)
{
	rgnset_h rh;
	Rect rect;

	rh = get_rgnset(stack->rgn);
	require(rh, end);

	topLeft(rect) = stack->top_left;
	botRight(rect) = stack->bottom_right;

	SetRectRgn_orig((**rh).copy, rect.left, rect.top, rect.right, rect.bottom);
	embiggen_rect(&rect);
	SetRectRgn_orig((**rh).big, rect.left, rect.top, rect.right, rect.bottom);

end:
	;
}
