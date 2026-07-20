/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#ifndef HIDPI_DISPOSERGN_PATCH_H
#define HIDPI_DISPOSERGN_PATCH_H

pascal void DisposeRgn_orig(RgnHandle rgn);
pascal void DisposeRgn_patch(void);

#endif
