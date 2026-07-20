/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#ifndef HIDPI_DISPOSERGN_PATCH
#define HIDPI_DISPOSERGN_PATCH

pascal void DisposeRgn_orig(RgnHandle rgn);
pascal void DisposeRgn_patch(void);

#endif
