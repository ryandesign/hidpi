/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#ifndef HIDPI_OFFSETRGN_PATCH_H
#define HIDPI_OFFSETRGN_PATCH_H

pascal void OffsetRgn_orig(RgnHandle rgn, short dh, short dv);
pascal void OffsetRgn_patch(void);

#endif
