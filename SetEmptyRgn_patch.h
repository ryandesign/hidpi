/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#ifndef HIDPI_SETEMPTYRGN_PATCH
#define HIDPI_SETEMPTYRGN_PATCH

pascal void SetEmptyRgn_orig(RgnHandle rgn);
pascal void SetEmptyRgn_patch(void);

#endif
