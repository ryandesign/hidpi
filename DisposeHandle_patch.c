/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#include "DisposeHandle_patch.h"

#include "CopyRgn_patch.h"
#include "globals.h"
#include "macros.h"
#include "rgnset.h"

void DisposeHandle_big(Handle h);

// On entry:
// A0: handle
// On exit:
// D0: result (word)

pascal void DisposeHandle_patch(void)
{
	asm
	{
		bsr.s 	@start							; Push token address; skip placeholders.
@token	dc.l	0								; Placeholder for globals token.
@orig	dc.l	k_placeholder					; Placeholder for old routine address.

@start	jsr		begin_globals					; Activate globals.
		addq.l	#4, sp							; Pop token address.

		movem.l	d0-d2/a1, -(sp)					; Save registers.
		move.l	a0, -(sp)						; Push handle / save A0.
		jsr		DisposeHandle_big				; Call our routine.
		movea.l	(sp)+, a0						; Pop handle / restore A0.
		movem.l	(sp)+, d0-d2/a1					; Restore registers.

		move.l	@token, -(sp)					; Push token.
		jsr		end_globals						; Deactivate globals.
		addq.l	#4, sp							; Pop token address.

		move.l	@orig, -(sp)					; Push old routine address.
		return									; "Return" to old routine.
	}
}

void DisposeHandle_big(Handle h)
{
	rgnset_p rp;

	require(h, end);
	require(g_data->closed_rgnbuf == h, end);

	g_data->closed_rgnbuf = nil;
	rp = *g_data->closed_rgnset;
	CopyRgn_orig(rp->original, rp->copy);

end:
	;
}
