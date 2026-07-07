/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#ifndef HIDPI_MISSING_TRAPS_H
#define HIDPI_MISSING_TRAPS_H

#include <Traps.h>

// Traps missing in THINK C 5

#ifndef _BlockMoveData
#define _BlockMoveData 0xA22E
#pragma parameter BlockMoveData(__A0,__A1,__D0)
pascal void BlockMoveData(const void *srcPtr,void *destPtr,Size byteCount)
	= _BlockMoveData;
#endif

#if 0
pascal long SetCurrentA5(void)
	= {0x2E8D,0x2A78,0x0904};
//	move.l  A5, (SP)        ; store old A5 as function result
//	move.l  CurrentA5, A5   ; set A5 to low memory global

pascal long SetA5(long newA5)
	= {0x2F4D,0x0004,0x2A5F};
//	move.l  A5, 4(SP)       ; store old A5 as function result
//	move.l  (SP)+, A5       ; set A5 to passed value
#endif

#endif
