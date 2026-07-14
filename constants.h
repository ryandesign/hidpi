/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#ifndef HIDPI_CONSTANTS
#define HIDPI_CONSTANTS

// Define USE_TRAP_PATCHING to patch traps instead of setting the 2x window's
// qdprocs. Patching traps is the way of the future. Many of this project's
// intended modifications require trap patching. The qdprocs method will go away
// once trap patching works completely and this project transitions into an
// INIT, but testing in an app is more convenient than having to restart every
// time I change the code. In an app, the full effect of trap patching can only
// be seen when running without MultiFinder, because under MultiFinder trap
// patches only affect the current app.
#define USE_TRAP_PATCHING

// TODO: finish implementing this
// The idea was to tie this to USE_TRAP_PATCHING so that the big cursor is still
// used when not trap patching, but its coordinates are still 1x.
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
