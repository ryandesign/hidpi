/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#include "patch_table.h"

#include <Traps.h>

#include "CloseRgn_patch.h"
#include "CopyRgn_patch.h"
#include "DisposeHandle_patch.h"
#include "DisposeRgn_patch.h"
#include "ExitToShell_patch.h"
#include "FillRect_patch.h"
#include "InitZone_patch.h"
#include "InsetRgn_patch.h"
#include "JCrsrObscure_patch.h"
#include "JInitCrsr_patch.h"
#include "JHideCursor_patch.h"
#include "JScrnSize_patch.h"
#include "JSetCrsr_patch.h"
#include "JShieldCursor_patch.h"
#include "JShowCursor_patch.h"
#include "OffsetRgn_patch.h"
#include "OpenRgn_patch.h"
#include "ScrnBitMap_patch.h"
#include "ScrollRect_patch.h"
#include "SectRgn_patch.h"
#include "SetEmptyRgn_patch.h"
#include "SystemTask_patch.h"

// Similar to "SP" for "stack pointer", "GP" is "globals pointer": the register
// containing the memory address from which globals are referenced -- either A4
// or A5 depending on how the code was compiled.
#if __option(a4_globals)
#define gp a4
#else
#define gp a5
#endif

#define begin_func(NAME) NAME:
#if __option(macsbug_names) && __option(long_macsbug_names)
#define end_func(NAME, RTS) RTS \
	dc.b 0x7F + sizeof(#NAME) \
	dc.b #NAME \
	dc.w 0
#else
#define end_func(NAME, RTS) RTS
#endif

// When building as a code resource, the Custom Header option must be turned on
// so that this function will be the first thing in the code resource.
#if __option(a4_globals)
void main(void)
#else
static void patch_table_and_globals(void)
#endif
{
	asm
	{
begin_func(get_storage)
		bsr.w 	@got_storage	; Push the address of the storage.
		dc.l 	0				; Storage for pointer to our globals.

extern begin_func(get_patch_table)
		bsr.w 	@got_patch_table; Push the address of the patch table.

		; The size of this function's code up to here must match the code_t
		; struct in globals.h. The size and layout of the patch table entries
		; below must match the patch_t struct in globals.h. Don't really need
		; jmp instructions here but can't figure out how else to get THINK C to
		; assemble the low-memory address using this symbol and the space used
		; by the jmp instruction is used by install().
		dc.l	_CloseRgn
		dc.w	CloseRgn_patch
		dc.l	_CopyRgn
		dc.w	CopyRgn_patch
		dc.l	_DisposHandle
		dc.w	DisposeHandle_patch
		dc.l	_DisposRgn
		dc.w	DisposeRgn_patch
#if !__option(a4_globals)
		dc.l	_ExitToShell
		dc.w	ExitToShell_patch
#endif
		dc.l	_FillRect
		dc.w	FillRect_patch
		dc.l	_InitZone
		dc.w	InitZone_patch
		dc.l	_InSetRgn
		dc.w	InsetRgn_patch
		jmp		JCrsrObscure
		dc.w	JCrsrObscure_patch
		jmp		JHideCursor
		dc.w	JHideCursor_patch
		jmp		JInitCrsr
		dc.w	JInitCrsr_patch
		jmp		JScrnSize
		dc.w	JScrnSize_patch
		;jmp		JSetCCrsr
		;dc.w	JSetCCrsr_patch
		jmp		JSetCrsr
		dc.w	JSetCrsr_patch
		jmp		JShieldCursor
		dc.w	JShieldCursor_patch
		jmp		JShowCursor
		dc.w	JShowCursor_patch
		dc.l	_OpenRgn
		dc.w	OpenRgn_patch
		dc.l	_OfSetRgn
		dc.w	OffsetRgn_patch
		dc.l	_ScrnBitMap
		dc.w	ScrnBitMap_patch
		dc.l	_ScrollRect
		dc.w	ScrollRect_patch
		dc.l	_SectRgn
		dc.w	SectRgn_patch
		dc.l	_SetEmptyRgn
		dc.w	SetEmptyRgn_patch
		dc.l	_SystemTask
		dc.w	SystemTask_patch
		dc.l	0				; End of patch table.

@got_patch_table:
		move.l	(sp)+, d0		; Pop the address into D0.
end_func(get_patch_table, rts)

@got_storage:
		movea.l	(sp)+, a0		; Pop the address into A0.
end_func(get_storage, rts)

#if !__option(a4_globals)
extern begin_func(link_globals)
		bsr 	@get_storage	; Move address of GP storage into A0.
		move.l	gp, (a0)		; Store GP in storage.
end_func(link_globals, rts)
#endif

extern begin_func(begin_globals)
		move.l	a0, -(sp)		; Save registers.
		movea.l	8(sp), a0		; Move address of token into A0.
		move.l	gp, (a0)		; Save GP in token.
		bsr 	@get_storage	; Move address of GP storage into A0.
		movea.l	(a0), gp		; Move stored GP into GP.
		movea.l	(sp)+, a0		; Restore registers.
end_func(begin_globals, rts)

extern begin_func(end_globals)
		move.l	4(sp), gp		; Restore GP from token.
end_func(end_globals, rts)
	}

	// The rts that THINK C inserts here is not reached.
}
