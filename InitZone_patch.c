/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#include "InitZone_patch.h"

#include "constants.h"

// On entry:
// A0: pointer to parameter block
// On exit:
// D0: result (word)

typedef struct
{
	Ptr startPtr;
	Ptr limitPtr;
	short cMoreMasters;
	Ptr pGrowZone;
}
InitZone_params;

// Since we create more handles in the app's space, make each block of
// master pointers slightly larger.
pascal void InitZone_patch(void)
{
	asm
	{
		bra		@start									; Skip placeholder.
@orig	dc.l	k_placeholder							; Placeholder for old routine address.

@start	move.l	d0, -(sp)								; Save registers.
		move.w	InitZone_params.cMoreMasters(a0), d0	; Get number of masters from block.
		lsr.w	#2, d0									; Divide by 4.
		add.w	d0, InitZone_params.cMoreMasters(a0)	; Add to num masters in block.

		move.l	(sp)+, d0								; Restore registers.

		move.l	@orig, -(sp)							; Push old routine address.
		return											; "Return" to old routine.
	}
}
