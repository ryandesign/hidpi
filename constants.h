/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#ifndef HIDPI_CONSTANTS
#define HIDPI_CONSTANTS

// TODO: finish implementing this
#undef SCALE_CURSOR_COORDS

// TODO: Fix more code to use k_scale instead of assuming it's 2.
// Would be nice to support 4 as well and maybe 3.
// TODO: runtime scale changes (restart required)
#define k_scale 2

#ifdef SCALE_CURSOR_COORDS
#define k_cursor_xy_multiplier k_scale
#define k_cursor_rect_multiplier 1
#else
#define k_cursor_xy_multiplier 1
#define k_cursor_rect_multiplier k_scale
#endif

#define k_pixels_per_byte 8

#define k_cursor_width 16
#define k_cursor_height 16
#define k_cursor_rowbytes (k_cursor_width / k_pixels_per_byte)
#define k_cursor_bytes (k_cursor_height * k_cursor_rowbytes)
#define k_cursor_rect_width (2 * k_cursor_width)
#define k_cursor_save_bytes (2 * k_cursor_bytes)

#define k_cursor_width_2x (k_cursor_width * k_scale)
#define k_cursor_height_2x (k_cursor_height * k_scale)
#define k_cursor_rowbytes_2x (k_cursor_rowbytes * k_scale)
#define k_cursor_bytes_2x (k_cursor_bytes * k_scale * k_scale)
#define k_cursor_rect_width_2x (k_cursor_rect_width * k_scale)
#define k_cursor_rowlongs_2x ((k_cursor_rowbytes_2x + 3) >> 2)
#define k_cursor_longs_2x (k_cursor_height_2x * k_cursor_rowlongs_2x)
#define k_cursor_save_rowlongs_2x (k_cursor_rowlongs_2x + 1)
#define k_cursor_save_longs_2x (k_cursor_height_2x * k_cursor_save_rowlongs_2x)

#define k_uninitialized_data 'NIL!'

#endif
