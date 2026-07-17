/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#ifndef HIDPI_MACROS_H
#define HIDPI_MACROS_H

#include "constants.h"

#define get_trap_type(TRAP) ((TRAP) & 0x0800 ? ToolTrap : OSTrap)

#define has_128k_rom() (ROM85 >= 0)

#define rect_width(RECT) ((RECT)->right - (RECT)->left)
#define rect_height(RECT) ((RECT)->bottom - (RECT)->top)

#define require(ASSERTION, EXCEPTION) \
	do { if (ASSERTION); else goto EXCEPTION; } while (false)

#define nrequire(ASSERTION, EXCEPTION) \
	do { if (!(ASSERTION)); else goto EXCEPTION; } while (false)

#define declare_asm(SYMBOL) \
		bra.s @0 \
@##SYMBOL: \
		dc.l k_placeholder \
@0:

#define declare(SYMBOL) \
	do { asm { declare_asm(SYMBOL) } } while (false)

#define jmp(ADDRESS) \
	do { asm { jmp (ADDRESS) } } while (false)

#define movea(SYMBOL, REGISTER) \
	do { asm { movea.l @##SYMBOL, REGISTER } } while (false)

#define push(SYMBOL) \
	do { asm { move.l @##SYMBOL, -(sp) } } while (false)

#define ror(COUNT, VAR) \
	do { asm { ror.l COUNT, VAR } } while (false)

#define save_regs() \
	do { asm { movem.l a0-a1/d0-d2, -(sp) } } while (false)

#define restore_regs() \
	do { asm { movem.l (sp)+, a0-a1/d0-d2 } } while (false)

#define sizeof_field(TYPE, MEMBER) sizeof((((TYPE *)0)->MEMBER))

#endif
