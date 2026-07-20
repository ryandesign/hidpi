/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#ifndef HIDPI_DEBIGULATE_H
#define HIDPI_DEBIGULATE_H

void debigulate(void);
void debigulate_later(short first_item_type, ...);
void debigulate_point(Point *point);
void debigulate_rect(Rect *rect);
void debigulate_rgn(RgnHandle rgn);
void debigulate_shorts(short *buf, short size);

#endif
