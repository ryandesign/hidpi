/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#include "FillRect_patch.h"

#include "globals.h"
#include "macros.h"

typedef struct
{
	long saved_registers[2];
	long return_address;
	long *pattern;
	Rect *rect;
} FillRect_stack;

// Fix window titlebar pattern.
// TODO: k_scale == 3 (could potentially use CopyBits to stretch the pattern)
pascal void FillRect_patch(void)
{
	// THINK C automatically saves the registers with movem.l here.

	declare(old_FillRect);

	{
		register Rect *rect; // AREG1

		asm
		{
			movea.l	FillRect_stack.rect(sp), rect		; Get address of rect.
		}

		require(&Scratch8 == rect, end);
	}

	{
		register long *pattern; // AREG1
		register long titlebar_pattern_1x; // DREG1

		asm
		{
			movea.l	FillRect_stack.pattern(sp), pattern	; Get address of pattern.
		}

		titlebar_pattern_1x = 0xFF00FF00;
		require(titlebar_pattern_1x == *pattern++, end);
		require(titlebar_pattern_1x == *pattern, end);

		asm
		{
			lea.l	@titlebar_pattern_big, pattern
			move.l	pattern, FillRect_stack.pattern(sp)	; Replace with bigger pattern.
		}
	}

end:
	{
		register long DREG1;
		register long *AREG1;

		asm
		{
			movem.l	(sp)+, DREG1/AREG1					; Restore registers.
			move.l	@old_FillRect, -(sp)				; Jump to old FillRect.
			rts
@titlebar_pattern_big:
#if k_scale == 4
			dc.l	0xFFFFFFFF
			dc.l	0x00000000
#else
			dc.l	0xFFFF0000
			dc.l	0xFFFF0000
#endif
		}
	}

	// The movem.l and rts that THINK C inserts here are not reached.
}
