/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#ifndef HIDPI_EMBIGGEN_H
#define HIDPI_EMBIGGEN_H

void embiggen_cursor(void);
void embiggen_point(Point *point);
void embiggen_rect(Rect *rect);
void embiggen_rgn(RgnHandle src, RgnHandle dst);

#endif
