/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#include "ScrollRect_patch.h"

#include "constants.h"
#include "debigulate.h"
#include "embiggen.h"
#include "globals.h"
#include "macros.h"

typedef struct
{
	long saved_registers[2];
	long token;
	long stack_frame;
	long return_address;
	RgnHandle update_rgn;
	short dv;
	short dh;
	Rect *rect;
} ScrollRect_stack;

pascal void ScrollRect_patch(void)
{
	// Because of the local variable, THINK C links a stack frame here.
	// THINK C automatically saves the registers with movem.l here.

	long token;

	declare(old_ScrollRect);

	begin_globals(&token);

	{
		register short delta; // DREG1

		asm
		{
			move.w	ScrollRect_stack.dh(sp), delta			; Get dh.
		}

		delta *= k_scale;

		asm
		{
			move.w	delta, ScrollRect_stack.dh(sp)			; Put it back.
			move.w	ScrollRect_stack.dv(sp), delta			; Get dv.
		}

		delta *= k_scale;

		asm
		{
			move.w	delta, ScrollRect_stack.dv(sp)			; Put it back.
		}
	}

	{
		register Rect *rect; // AREG1
		register long rgn; // DREG1

		asm
		{
			move.l	ScrollRect_stack.update_rgn(sp), rgn	; Get address of update region.
			movea.l	ScrollRect_stack.rect(sp), rect			; Get address of rect.
		}

		embiggen_rect(rect);
		debigulate_later(
			t_RectPtr, rect, nil,
			t_RgnHandle, (RgnHandle)rgn, nil,
			t_none);
	}

	end_globals(token);

	{
		register long DREG1;
		register long *AREG1;

		asm
		{
			movem.l (sp)+, DREG1/AREG1						; Restore registers.
			unlk a6											; Unlink stack frame.
			move.l	@old_ScrollRect, -(sp)					; Jump to old ScrollRect.
			rts
		}
	}

	// The movem.l, unlk, and rts that THINK C inserts here are not reached.
}
