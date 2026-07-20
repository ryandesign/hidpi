/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#ifndef HIDPI_COPYRGN_PATCH
#define HIDPI_COPYRGN_PATCH

pascal void CopyRgn_orig(RgnHandle src, RgnHandle dst);
pascal void CopyRgn_patch(void);

#endif
