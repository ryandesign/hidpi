/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#include "InsetOffsetRgn_patch.h"

#include "embiggen.h"
#include "globals.h"
#include "macros.h"
#include "rgnset.h"

typedef pascal void (*IORgn_proc_ptr)(RgnHandle, short, short);

typedef struct
{
	IORgn_proc_ptr IORgn_orig;
	long return_address;
	Point delta;
	RgnHandle rgn;
}
IORgn_stack;

static void InsetOffsetRgn_big(IORgn_stack *stack, IORgn_proc_ptr IORgn_orig);

static pascal void InsetOffsetRgn_patch(void)
{
	asm
	{
extern InsetRgn_patch:
		bsr.s 	@deref							; Push old routine pointer; skip placeholder.
@orig_I	dc.l	k_placeholder					; Placeholder for old routine address.

extern InsetRgn_orig:
		move.l	@orig_I, -(sp)					; Push old routine address.
		rts										; "Return" to old routine.

extern OffsetRgn_patch:
		bsr.s 	@deref							; Push old routine pointer; skip placeholder.
@orig_O	dc.l	k_placeholder					; Placeholder for old routine address.

extern OffsetRgn_orig:
		move.l	@orig_O, -(sp)					; Push old routine address.
		rts										; "Return" to old routine.

@deref	move.l	(sp), a0						; "Pop" old routine pointer.
		move.l	(a0), (sp)						; "Push" old routine address.

		bsr.s 	@start							; Push token address; skip placeholders.
@token	dc.l	0								; Placeholder for globals token.

@start	jsr		begin_globals					; Activate globals.
		addq.l	#4, sp							; Pop token address.

		move.l	sp, -(sp)						; Push stack pointer.
		jsr		InsetOffsetRgn_big				; Call our routine.
		addq.l	#4, sp							; Pop stack pointer.

		move.l	@token, -(sp)					; Push token.
		jsr		end_globals						; Deactivate globals.
		addq.l	#4, sp							; Pop token.

		return									; "Return" to old routine.
	}
}

static void InsetOffsetRgn_big(IORgn_stack *stack, IORgn_proc_ptr IORgn_orig)
{
	rgnset_h rh;
	Point delta = stack->delta;

	rh = get_rgnset(stack->rgn);
	require(rh, end);

	IORgn_orig((**rh).copy, delta.h, delta.v);
	embiggen_point(&delta);
	IORgn_orig((**rh).big, delta.h, delta.v);

end:
	;
}
