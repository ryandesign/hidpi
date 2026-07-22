/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#ifndef HIDPI_INSETOFFSETRGN_PATCH_H
#define HIDPI_INSETOFFSETRGN_PATCH_H

pascal void InsetRgn_orig(RgnHandle rgn, short dh, short dv);
pascal void InsetRgn_patch(void);
pascal void OffsetRgn_orig(RgnHandle rgn, short dh, short dv);
pascal void OffsetRgn_patch(void);

#endif
