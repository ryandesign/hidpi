/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#ifndef HIDPI_CURSOR_STUFF
#define HIDPI_CURSOR_STUFF

void get_screen_rect(Rect *rect);
void pin_hotspot(Point *hotspot);
void sync_hotspot_2x(void);
void load_cursor_2x(void);

#endif
