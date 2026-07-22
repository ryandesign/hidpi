/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#ifndef HIDPI_RECTRGN_PATCH_H
#define HIDPI_RECTRGN_PATCH_H

pascal void RectRgn_orig(RgnHandle rgn, Rect *rect);
pascal void RectRgn_patch(void);

#endif
