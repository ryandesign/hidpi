/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#ifndef HIDPI_CLOSERGN_PATCH_H
#define HIDPI_CLOSERGN_PATCH_H

pascal void CloseRgn_orig(RgnHandle rgn);
pascal void CloseRgn_patch(void);

#endif
