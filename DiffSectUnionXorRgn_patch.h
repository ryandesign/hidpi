/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#ifndef HIDPI_DIFFSECTUNIONXORRGN_PATCH_H
#define HIDPI_DIFFSECTUNIONXORRGN_PATCH_H

pascal void DiffRgn_orig(RgnHandle src1, RgnHandle src2, RgnHandle dst);
pascal void DiffRgn_patch(void);
pascal void SectRgn_orig(RgnHandle src1, RgnHandle src2, RgnHandle dst);
pascal void SectRgn_patch(void);
pascal void UnionRgn_orig(RgnHandle src1, RgnHandle src2, RgnHandle dst);
pascal void UnionRgn_patch(void);
pascal void XorRgn_orig(RgnHandle src1, RgnHandle src2, RgnHandle dst);
pascal void XorRgn_patch(void);

#endif
