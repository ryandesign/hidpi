/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#ifndef HIDPI_MACROS_H
#define HIDPI_MACROS_H

#define get_trap_type(TRAP) ((TRAP) & 0x0800 ? ToolTrap : OSTrap)

#define rect_width(RECT) ((RECT)->right - (RECT)->left)
#define rect_height(RECT) ((RECT)->bottom - (RECT)->top)

#endif
