/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#include "debug.h"

void redraw_screen(void) {
	PaintBehind((WindowPeek)FrontWindow(), GetGrayRgn());
	HiliteMenu(0);
	DrawMenuBar();
}
