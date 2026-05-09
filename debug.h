/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#ifndef HIDPI_DEBUG_H
#define HIDPI_DEBUG_H

void debug_numer_denom(StringPtr message, Point numer, Point denom);
void debug_font_info(StringPtr message, FontInfo *info);
void debug_ptr(StringPtr message, Ptr ptr);
void debug_hex(StringPtr message, unsigned long hex);
void debug_num(StringPtr message, long num);
void debug_rect(StringPtr message, Rect *rect);
Boolean equal_buf(short count1, Ptr buf1, short count2, Ptr buf2);
void debug_buf(StringPtr message, short size, Ptr buf);
void debug_bits(StringPtr message, BitMap *src_bits, Rect *src_rect, Boolean src_hi, BitMap *dst_bits, Rect *dst_rect, Boolean dst_hi, RgnHandle mask_rgn);
void print_event(EventRecord *event);
void redraw_screen(void);

#endif
