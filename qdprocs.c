/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#include "qdprocs.h"

#define k_rgn_end_flag 0x7FFF

QDProcs g_std_qdprocs;
QDProcs g_my_qdprocs;

static Boolean is_port_2x(void) {
	GrafPtr port;

	GetPort(&port);
	return nil == port->picSave && nil == port->rgnSave && nil == port->polySave;
}

static void double_shorts(short *buf, int count) {
	int i;

	for (i = 0; i < count; ++i) {
		if (k_rgn_end_flag != buf[i]) {
			buf[i] <<= 1;
		}
	}
}

static void double_point(Point *src_point, Point *dst_point) {
	dst_point->h = src_point->h << 1;
	dst_point->v = src_point->v << 1;
}

static void half_point(Point *src_point, Point *dst_point) {
	dst_point->h = src_point->h >> 1;
	dst_point->v = src_point->v >> 1;
}

static void double_rect(Rect *src_rect, Rect *dst_rect) {
	dst_rect->left = src_rect->left << 1;
	dst_rect->top = src_rect->top << 1;
	dst_rect->right = src_rect->right << 1;
	dst_rect->bottom = src_rect->bottom << 1;
}

typedef pascal void (*TextProcPtr)(short, Ptr, Point, Point);
static pascal void text_2x(short byte_count, Ptr text_buf, Point numer, Point denom) {
	PenState pen;

	if (is_port_2x()) {
		// TODO: double spExtra
		// TODO: deduplicate pen size code
		double_point(&numer, &numer);
		GetPenState(&pen);
		Move(pen.pnLoc.h, pen.pnLoc.v);
		((TextProcPtr)g_std_qdprocs.textProc)(byte_count, text_buf, numer, denom);
		GetPenState(&pen);
		MoveTo(pen.pnLoc.h >> 1, pen.pnLoc.v >> 1);
	} else {
		((TextProcPtr)g_std_qdprocs.textProc)(byte_count, text_buf, numer, denom);
	}
}

typedef pascal void (*LineProcPtr)(Point);
static pascal void line_2x(Point end_point) {
	PenState pen;
	Point end_point_2x;

	if (is_port_2x()) {
		double_point(&end_point, &end_point_2x);
		GetPenState(&pen);
		PenSize(pen.pnSize.h << 1, pen.pnSize.v << 1);
		Move(pen.pnLoc.h, pen.pnLoc.v);
		((LineProcPtr)g_std_qdprocs.lineProc)(end_point_2x);
		PenSize(pen.pnSize.h, pen.pnSize.v);
		MoveTo(end_point.h, end_point.v);
	} else {
		((LineProcPtr)g_std_qdprocs.lineProc)(end_point);
	}
}

typedef pascal void (*RectProcPtr)(GrafVerb, Rect);
static pascal void rect_2x(GrafVerb verb, Rect rect) {
	PenState pen;

	if (is_port_2x()) {
		double_rect(&rect, &rect);
		GetPenState(&pen);
		PenSize(pen.pnSize.h << 1, pen.pnSize.v << 1);
		((RectProcPtr)g_std_qdprocs.rectProc)(verb, rect);
		PenSize(pen.pnSize.h, pen.pnSize.v);
	} else {
		((RectProcPtr)g_std_qdprocs.rectProc)(verb, rect);
	}
}

typedef pascal void (*RRectProcPtr)(GrafVerb, Rect, short, short);
static pascal void rrect_2x(GrafVerb verb, Rect rect, short oval_width, short oval_height) {
	PenState pen;

	if (is_port_2x()) {
		double_rect(&rect, &rect);
		GetPenState(&pen);
		PenSize(pen.pnSize.h << 1, pen.pnSize.v << 1);
		((RRectProcPtr)g_std_qdprocs.rRectProc)(verb, rect, oval_width << 1, oval_height << 1);
		PenSize(pen.pnSize.h, pen.pnSize.v);
	} else {
		((RRectProcPtr)g_std_qdprocs.rRectProc)(verb, rect, oval_width, oval_height);
	}
}

typedef pascal void (*OvalProcPtr)(GrafVerb, Rect);
static pascal void oval_2x(GrafVerb verb, Rect rect) {
	PenState pen;

	if (is_port_2x()) {
		double_rect(&rect, &rect);
		GetPenState(&pen);
		PenSize(pen.pnSize.h << 1, pen.pnSize.v << 1);
		((OvalProcPtr)g_std_qdprocs.ovalProc)(verb, rect);
		PenSize(pen.pnSize.h, pen.pnSize.v);
	} else {
		((OvalProcPtr)g_std_qdprocs.ovalProc)(verb, rect);
	}
}

typedef pascal void (*ArcProcPtr)(GrafVerb, Rect, short, short);
static pascal void arc_2x(GrafVerb verb, Rect rect, short start_angle, short arc_angle) {
	PenState pen;

	if (is_port_2x()) {
		double_rect(&rect, &rect);
		GetPenState(&pen);
		PenSize(pen.pnSize.h << 1, pen.pnSize.v << 1);
		((ArcProcPtr)g_std_qdprocs.arcProc)(verb, rect, start_angle, arc_angle);
		PenSize(pen.pnSize.h, pen.pnSize.v);
	} else {
		((ArcProcPtr)g_std_qdprocs.arcProc)(verb, rect, start_angle, arc_angle);
	}
}

typedef pascal void (*PolyProcPtr)(GrafVerb, PolyHandle);
static pascal void poly_2x(GrafVerb verb, PolyHandle poly) {
	PenState pen;
	PolyHandle poly_2x;
	PolyPtr poly_2x_ptr;

	if (is_port_2x()) {
		poly_2x = poly;
		if (noErr == HandToHand((Handle *)&poly_2x)) {
			HLock((Handle)poly_2x);
			poly_2x_ptr = *poly_2x;
			double_shorts((short *)&poly_2x_ptr->polyBBox, (poly_2x_ptr->polySize - sizeof(short)) / sizeof(short));
			HUnlock((Handle)poly_2x);
			GetPenState(&pen);
			PenSize(pen.pnSize.h << 1, pen.pnSize.v << 1);
			((PolyProcPtr)g_std_qdprocs.polyProc)(verb, poly_2x);
			PenSize(pen.pnSize.h, pen.pnSize.v);
			DisposeHandle((Handle)poly_2x);
		}
	} else {
		((PolyProcPtr)g_std_qdprocs.polyProc)(verb, poly);
	}
}

typedef pascal void (*RgnProcPtr)(GrafVerb, RgnHandle);
static pascal void rgn_2x(GrafVerb verb, RgnHandle rgn) {
	PenState pen;
	RgnHandle rgn_2x;
	RgnPtr rgn_2x_ptr;

	if (is_port_2x()) {
		rgn_2x = rgn;
		if (noErr == HandToHand((Handle *)&rgn_2x)) {
			HLock((Handle)rgn_2x);
			rgn_2x_ptr = *rgn_2x;
			double_shorts((short *)&rgn_2x_ptr->rgnBBox, (rgn_2x_ptr->rgnSize - sizeof(short)) / sizeof(short));
			HUnlock((Handle)rgn_2x);
			GetPenState(&pen);
			PenSize(pen.pnSize.h << 1, pen.pnSize.v << 1);
			((RgnProcPtr)g_std_qdprocs.rgnProc)(verb, rgn_2x);
			PenSize(pen.pnSize.h, pen.pnSize.v);
			DisposeHandle((Handle)rgn_2x);
		}
	} else {
		((RgnProcPtr)g_std_qdprocs.rgnProc)(verb, rgn);
	}
}

typedef pascal void (*BitsProcPtr)(BitMap *, Rect *, Rect *, short, RgnHandle);
static pascal void bits_2x(BitMap *src_bits, Rect *src_rect, Rect *dst_rect, short mode, RgnHandle mask_rgn) {
	Rect dst_rect_2x;

	if (is_port_2x()) {
		double_rect(dst_rect, &dst_rect_2x);
		((BitsProcPtr)g_std_qdprocs.bitsProc)(src_bits, src_rect, &dst_rect_2x, mode, mask_rgn);
	} else {
		((BitsProcPtr)g_std_qdprocs.bitsProc)(src_bits, src_rect, dst_rect, mode, mask_rgn);
	}
}

typedef pascal short (*TxMeasProcPtr)(short, Ptr, Point *, Point *, FontInfo *);
static pascal short txmeas_2x(short byte_count, Ptr text_buf, Point *numer, Point *denom, FontInfo *info) {
	short width;

	if (is_port_2x()) {
		double_point(numer, numer);
		width = ((TxMeasProcPtr)g_std_qdprocs.txMeasProc)(byte_count, text_buf, numer, denom, info);
		half_point(numer, numer);
	} else {
		width = ((TxMeasProcPtr)g_std_qdprocs.txMeasProc)(byte_count, text_buf, numer, denom, info);
	}

	return width;
}

void init_qdprocs(void) {
	SetStdProcs(&g_std_qdprocs);
	SetStdProcs(&g_my_qdprocs);
	g_my_qdprocs.textProc = (Ptr)&text_2x;
	g_my_qdprocs.lineProc = (Ptr)&line_2x;
	g_my_qdprocs.rectProc = (Ptr)&rect_2x;
	g_my_qdprocs.rRectProc = (Ptr)&rrect_2x;
	g_my_qdprocs.ovalProc = (Ptr)&oval_2x;
	g_my_qdprocs.arcProc = (Ptr)&arc_2x;
	g_my_qdprocs.polyProc = (Ptr)&poly_2x;
	g_my_qdprocs.rgnProc = (Ptr)&rgn_2x;
	g_my_qdprocs.bitsProc = (Ptr)&bits_2x;
	g_my_qdprocs.txMeasProc = (Ptr)&txmeas_2x;
}

// TODO: handle origin
// TODO: don't double to more than 32766
// TODO: handle cliprgn
