/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#ifndef HIDPI_INSETRGN_PATCH
#define HIDPI_INSETRGN_PATCH

pascal void InsetRgn_orig(RgnHandle rgn, short dh, short dv);
pascal void InsetRgn_patch(void);

#endif
