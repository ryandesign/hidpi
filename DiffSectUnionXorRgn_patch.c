/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#include "DiffSectUnionXorRgn_patch.h"

#include "globals.h"
#include "macros.h"
#include "rgnset.h"

typedef pascal void (*DSUXRgn_proc_ptr)(RgnHandle, RgnHandle, RgnHandle);

typedef struct
{
	DSUXRgn_proc_ptr DSUXRgn_orig;
	long return_address;
	RgnHandle dst;
	RgnHandle src2;
	RgnHandle src1;
}
DSUXRgn_stack;

static void DiffSectUnionXorRgn_big(DSUXRgn_stack *stack, DSUXRgn_proc_ptr DSUXRgn_orig);

static pascal void DiffSectUnionXorRgn_patch(void)
{
	asm
	{
extern DiffRgn_orig:
		move.l	@orig_D, -(sp)					; Push old routine address.
		rts										; "Return" to old routine.

extern DiffRgn_patch:
		bsr 	@deref							; Push old routine pointer; skip placeholder.
@orig_D	dc.l	k_placeholder					; Placeholder for old routine address.

extern SectRgn_orig:
		move.l	@orig_S, -(sp)					; Push old routine address.
		rts										; "Return" to old routine.

extern SectRgn_patch:
		bsr 	@deref							; Push old routine pointer; skip placeholder.
@orig_S	dc.l	k_placeholder					; Placeholder for old routine address.

extern UnionRgn_orig:
		move.l	@orig_U, -(sp)					; Push old routine address.
		rts										; "Return" to old routine.

extern UnionRgn_patch:
		bsr 	@deref							; Push old routine pointer; skip placeholder.
@orig_U	dc.l	k_placeholder					; Placeholder for old routine address.

extern XorRgn_orig:
		move.l	@orig_X, -(sp)					; Push old routine address.
		rts										; "Return" to old routine.

extern XorRgn_patch:
		bsr 	@deref							; Push old routine pointer; skip placeholder.
@orig_X	dc.l	k_placeholder					; Placeholder for old routine address.

@deref	move.l	(sp), a0						; "Pop" old routine pointer.
		move.l	(a0), (sp)						; "Push" old routine address.

		bsr 	@start							; Push token address; skip placeholder.
@token	dc.l	0								; Placeholder for globals token.

@start	jsr		begin_globals					; Activate globals.
		addq.l	#4, sp							; Pop token address.

		move.l	sp, -(sp)						; Push stack pointer.
		jsr		DiffSectUnionXorRgn_big			; Call our routine.
		addq.l	#4, sp							; Pop stack pointer.

		move.l	@token, -(sp)					; Push token.
		jsr		end_globals						; Deactivate globals.
		addq.l	#4, sp							; Pop token.

		return									; "Return" to old routine.
	}
}

static void DiffSectUnionXorRgn_big(DSUXRgn_stack *stack, DSUXRgn_proc_ptr DSUXRgn_orig)
{
	rgnset_h src1rh, src2rh, dstrh;

	src1rh = get_rgnset(stack->src1);
	require(src1rh, end);

	src2rh = get_rgnset(stack->src2);
	require(src2rh, end);

	dstrh = get_rgnset(stack->dst);
	require(dstrh, end);

	DSUXRgn_orig((**src1rh).copy, (**src2rh).copy, (**dstrh).copy);
	DSUXRgn_orig((**src1rh).big, (**src2rh).big, (**dstrh).big);

end:
	;
}
