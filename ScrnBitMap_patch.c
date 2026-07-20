/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#include "ScrnBitMap_patch.h"

#include "constants.h"
#include "debigulate.h"
#include "embiggen.h"
#include "globals.h"
#include "macros.h"

typedef struct
{
	long return_address;
	BitMap *bitmap;
}
ScrnBitMap_stack;

static void ScrnBitMap_big(BitMap *bitmap);

pascal void ScrnBitMap_patch(void)
{
	asm
	{
		bra 	@start								; Skip placeholder.
@orig	dc.l	k_placeholder						; Placeholder for old routine address.

@start	move.l	ScrnBitMap_stack.bitmap(sp), -(sp)	; Push bitmap pointer.
		jsr		ScrnBitMap_big						; Call our routine.
		addq.l	#4, sp								; Pop bitmap pointer.

		move.l	(sp)+, (sp)							; Pop caller's parameters.
		rts											; Return to caller.

extern ScrnBitMap_orig:
		move.l	@orig, -(sp)						; Push old routine address.
		return										; "Return" to old routine.
	}
}

static void ScrnBitMap_big(BitMap *bitmap)
{
	ScrnBitMap_orig(bitmap);
	debigulate_rect(&bitmap->bounds);
}
