/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#ifndef HIDPI_SETRECTRGN_PATCH_H
#define HIDPI_SETRECTRGN_PATCH_H

pascal void SetRectRgn_orig(RgnHandle rgn, short left, short top, short right, short bottom);
pascal void SetRectRgn_patch(void);

#endif
