/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#ifndef HIDPI_SECTRGN_PATCH_H
#define HIDPI_SECTRGN_PATCH_H

pascal void SectRgn_orig(RgnHandle src1, RgnHandle src2, RgnHandle dst);
pascal void SectRgn_patch(void);

#endif
