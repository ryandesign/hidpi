/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#include "debug.h"

#define k_debug_event_width 128
#define k_line_height 16

// pstrlcat is a Pascal version of strlcat, which appends one string to another.
// src and dst are Pascal strings. size is the size of dst in bytes including
// the length byte. pstrlcat will append at most size - dst[0] - 1 bytes, and
// also caps the final string length at 255 characters.
static Size pstrlcat(StringPtr dst, StringPtr src, Size size) {
	Size intended_len;
	Size max_len;
	Size actual_len;
	int src_index = 0;
	int dst_index;

	intended_len = src[0] + dst[0];

	max_len = size - 1;
	if (max_len > 255) {
		max_len = 255;
	}

	if (intended_len > max_len) {
		actual_len = max_len;
	} else {
		actual_len = intended_len;
	}

	dst_index = dst[0];
	while (dst_index < actual_len) {
		dst[++dst_index] = src[++src_index];
	}
	dst[0] = actual_len;

	return intended_len;
}

// TODO: this all ends up being rather verbose, doesn't it? simplify with
//       dedicated functions that append different types to a string

static StringPtr num_to_str(long num, StringPtr dst, Size size) {
	Str15 tmp;

	dst[0] = 0;

	NumToString(num, tmp);
	pstrlcat(dst, tmp, size);

	return dst;
}

static StringPtr pt_to_str(Point pt, StringPtr dst, Size size) {
	Str15 tmp;

	dst[0] = tmp[0] = 0;

	pstrlcat(dst, "\p(", size);
	pstrlcat(dst, num_to_str(pt.h, tmp, 16), size);
	pstrlcat(dst, "\p,", size);
	pstrlcat(dst, num_to_str(pt.v, tmp, 16), size);
	pstrlcat(dst, "\p)", size);

	return dst;
}

static StringPtr rect_to_str(Rect *rect, StringPtr dst, Size size) {
	Str15 tmp;

	dst[0] = tmp[0] = 0;

	pstrlcat(dst, "\p(", size);
	pstrlcat(dst, num_to_str(rect->left, tmp, 16), size);
	pstrlcat(dst, "\p,", size);
	pstrlcat(dst, num_to_str(rect->top, tmp, 16), size);
	pstrlcat(dst, "\p,", size);
	pstrlcat(dst, num_to_str(rect->right, tmp, 16), size);
	pstrlcat(dst, "\p,", size);
	pstrlcat(dst, num_to_str(rect->bottom, tmp, 16), size);
	pstrlcat(dst, "\p)", size);

	return dst;
}

static StringPtr numer_denom_to_str(Point numer, Point denom, StringPtr dst, Size size) {
	Str15 tmp;

	dst[0] = tmp[0] = 0;

	pstrlcat(dst, "\p(", size);
	pstrlcat(dst, num_to_str(numer.h, tmp, 16), size);
	pstrlcat(dst, "\p/", size);
	pstrlcat(dst, num_to_str(denom.h, tmp, 16), size);
	pstrlcat(dst, "\p,", size);
	pstrlcat(dst, num_to_str(numer.v, tmp, 16), size);
	pstrlcat(dst, "\p/", size);
	pstrlcat(dst, num_to_str(denom.v, tmp, 16), size);
	pstrlcat(dst, "\p)", size);

	return dst;
}

static StringPtr font_info_to_str(FontInfo *info, StringPtr dst, Size size) {
	Str63 tmp;

	dst[0] = tmp[0] = 0;

	pstrlcat(dst, "\p[asc:", size);
	pstrlcat(dst, num_to_str(info->ascent, tmp, 64), size);
	pstrlcat(dst, "\p,dec:", size);
	pstrlcat(dst, num_to_str(info->descent, tmp, 64), size);
	pstrlcat(dst, "\p,widMax:", size);
	pstrlcat(dst, num_to_str(info->widMax, tmp, 64), size);
	pstrlcat(dst, "\p,lead:", size);
	pstrlcat(dst, num_to_str(info->leading, tmp, 64), size);
	pstrlcat(dst, "\p]", size);

	return dst;
}

void debug_font_info(StringPtr message, FontInfo *info) {
	Str255 dst;
	Str63 tmp;

	dst[0] = tmp[0] = 0;

	pstrlcat(dst, message, 256);
	pstrlcat(dst, font_info_to_str(info, tmp, 64), 256);

	DebugStr(dst);
}

void debug_numer_denom(StringPtr message, Point numer, Point denom) {
	Str255 dst;
	Str31 tmp;

	dst[0] = tmp[0] = 0;

	pstrlcat(dst, message, 256);
	pstrlcat(dst, numer_denom_to_str(numer, denom, tmp, 32), 256);

	DebugStr(dst);
}

static unsigned char count_digits(unsigned long num, char base) {
	unsigned char len = 0;

	do {
		++len;
		num /= base;
	} while (num > 0);

	return len;
}

// TODO: add and respect size
static StringPtr num_to_hex_str(unsigned long num, StringPtr dst, unsigned char len) {
	unsigned char hex;

	if (0 == len) {
		len = count_digits(num, 0x10);
	}
	dst[0] = len;

	while (len > 0) {
		hex = num % 0x10;
		hex += hex >= 0xA ? ('A' - 0xA) : '0';
		dst[len--] = hex;
		num /= 0x10;
	}

	return dst;
}

// TODO: respect size
static StringPtr ptr_to_str(Ptr ptr, StringPtr str, Size size) {
	return num_to_hex_str((unsigned long)StripAddress(ptr), str, 2 * sizeof(Ptr));
}

void debug_ptr(StringPtr message, Ptr ptr) {
	Str255 dst;
	Str15 tmp;

	dst[0] = tmp[0] = 0;

	pstrlcat(dst, message, 256);
	pstrlcat(dst, ptr_to_str(ptr, tmp, 16), 256);

	DebugStr(dst);
}

void debug_hex(StringPtr message, unsigned long hex) {
	Str255 dst;
	Str15 tmp;

	dst[0] = tmp[0] = 0;

	pstrlcat(dst, message, 256);
	pstrlcat(dst, num_to_hex_str(hex, tmp, 0), 256);

	DebugStr(dst);
}

void debug_num(StringPtr message, long num) {
	Str255 dst;
	Str15 tmp;

	dst[0] = tmp[0] = 0;

	pstrlcat(dst, message, 256);
	pstrlcat(dst, num_to_str(num, tmp, 16), 256);

	DebugStr(dst);
}

void debug_rect(StringPtr message, Rect *rect) {
	Str255 dst;
	Str31 tmp;

	dst[0] = tmp[0] = 0;

	pstrlcat(dst, message, 256);
	pstrlcat(dst, rect_to_str(rect, tmp, 32), 256);

	DebugStr(dst);
}

Boolean equal_buf(short count1, Ptr buf1, short count2, Ptr buf2) {
	short i;

	if (count1 != count2) {
		return false;
	}

	for (i = 0; i < count1; ++i) {
		if (buf1[i] != buf2[i]) {
			return false;
		}
	}

	return true;
}

void debug_buf(StringPtr message, short size, Ptr buf) {
	Str255 dst;
	short i = 0;
	unsigned char c;

	if (size > 255) {
		size = 255;
	}
	dst[0] = size;

	while (i < size) {
		c = buf[i];
		dst[++i] = c;
	}

	DebugStr(dst);
}

void debug_bits(StringPtr message, BitMap *src_bits, Rect *src_rect, Boolean src_hi, BitMap *dst_bits, Rect *dst_rect, Boolean dst_hi, RgnHandle mask_rgn) {
	Str255 dst;
	Str31 tmp;

	dst[0] = tmp[0] = 0;

	pstrlcat(dst, message, 256);
	pstrlcat(dst, ptr_to_str(src_bits->baseAddr, tmp, 32), 256);
	pstrlcat(dst, rect_to_str(src_rect, tmp, 32), 256);
	if (src_hi) {
		pstrlcat(dst, "\p@2x", 256);
	}
	pstrlcat(dst, "\p->", 256);
	pstrlcat(dst, ptr_to_str(dst_bits->baseAddr, tmp, 32), 256);
	pstrlcat(dst, rect_to_str(dst_rect, tmp, 32), 256);
	if (dst_hi) {
		pstrlcat(dst, "\p@2x", 256);
	}
	if (nil == mask_rgn) {
		pstrlcat(dst, "\p nil", 256);
	} else {
		pstrlcat(dst, rect_to_str(&(**mask_rgn).rgnBBox, tmp, 32), 256);
	}

	DebugStr(dst);
}

// TODO: rewrite by concatenating using above functions and drawing string once
void print_event(EventRecord *event) {
	GrafPtr saved_port;
	GrafPtr port;
	Rect rect;
	short mbar_height;
	RgnHandle saved_clip;
	RgnHandle rgn;
	static count = 0;
	Str255 str;

	GetPort(&saved_port);
	GetWMgrPort(&port);
	SetPort(port);

	rect = port->portRect;
	rect.top = rect.bottom - count * k_line_height;
	mbar_height = GetMBarHeight();
	if (rect.top < mbar_height) {
		rect.top = mbar_height;
	}
	rect.left = rect.right - k_debug_event_width;

	saved_clip = NewRgn();
	if (nil != saved_clip) {
		GetClip(saved_clip);
		ClipRect(&rect);
	}

	rgn = NewRgn();
	if (nil != rgn) {
		ScrollRect(&rect, 0, -k_line_height, rgn);
		DisposeRgn(rgn);
	}

	MoveTo(port->portRect.right - k_debug_event_width, port->portRect.bottom - 4);
	NumToString(count++, str);
	DrawString(str);
	DrawString("\p ");
	switch (event->what) {
		case nullEvent:
			DrawString("\pnull");
			break;
		case mouseDown:
			DrawString("\pmouseDown");
			break;
		case mouseUp:
			DrawString("\pmouseUp");
			break;
		case keyDown:
			DrawString("\pkeyDown");
			break;
		case autoKey:
			DrawString("\pautoKey");
			break;
		case activateEvt:
			if (event->modifiers & activeFlag) {
				DrawString("\pactivate");
			} else {
				DrawString("\pdeactivate");
			}
			break;
		case updateEvt:
			DrawString("\pupdate");
			break;
		case diskEvt:
			DrawString("\pdisk");
			break;
		case osEvt:
			switch ((event->message >> 24) & 0xFF) {
				case mouseMovedMessage:
					DrawString("\pmouseMoved");
					break;
				case suspendResumeMessage:
					if (event->message & resumeFlag) {
						DrawString("\presume");
					} else {
						DrawString("\psuspend");
					}
					break;
				default:
					DrawString("\pos");
			}
			break;
		default:
			NumToString(event->what, str);
			DrawString(str);
			break;
	}

	if (nil != saved_clip) {
		SetClip(saved_clip);
		DisposeRgn(saved_clip);
	}

	SetPort(saved_port);
}

void redraw_screen(void) {
	PaintBehind((WindowPeek)FrontWindow(), GetGrayRgn());
	HiliteMenu(0);
	DrawMenuBar();
}

// TODO: use Size in more places?
