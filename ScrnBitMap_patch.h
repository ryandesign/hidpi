/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#ifndef HIDPI_SCRNBITMAP_PATCH
#define HIDPI_SCRNBITMAP_PATCH

pascal void ScrnBitMap_patch(BitMap *bitmap);
void ScrnBitMap_2x(BitMap *bitmap);

#endif