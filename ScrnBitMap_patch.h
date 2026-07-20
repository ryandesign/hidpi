/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#ifndef HIDPI_SCRNBITMAP_PATCH_H
#define HIDPI_SCRNBITMAP_PATCH_H

pascal void ScrnBitMap_orig(BitMap *bitmap);
pascal void ScrnBitMap_patch(void);

#endif
