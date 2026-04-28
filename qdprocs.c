/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#include "qdprocs.h"

#include <Traps.h>

#undef USE_TRAP_PATCHING

#ifdef USE_TRAP_PATCHING
	#include <SetUpA4.h>
	#if !__option(a4_globals)
		#define a4 a5
	#endif
	#define remember_globals() RememberA4()
	#define start_accessing_globals() SetUpA4()
	#define stop_accessing_globals() RestoreA4()
#else
	#define remember_globals()
	#define start_accessing_globals()
	#define stop_accessing_globals()
#endif

//#define k_port_is_2x 1
#define k_rgn_end_flag 0x7FFF

// A polygon or a region.
typedef struct object {
	short size; // total bytes including size, bbox, and data
	Rect bbox;
	short data[1]; // 0 or more QuickDraw h/v values
} object;

typedef object *obj_ptr, **obj_handle;

typedef pascal void (*TextProcPtr)(short, Ptr, Point, Point);
typedef pascal void (*LineProcPtr)(Point);
typedef pascal void (*RectOrOvalProcPtr)(GrafVerb, Rect);
typedef RectOrOvalProcPtr RectProcPtr;
typedef RectOrOvalProcPtr OvalProcPtr;
typedef pascal void (*RRectOrArcProcPtr)(GrafVerb, Rect, short, short);
typedef RRectOrArcProcPtr RRectProcPtr;
typedef RRectOrArcProcPtr ArcProcPtr;
typedef pascal void (*PolyOrRgnProcPtr)(GrafVerb, obj_handle);
typedef PolyOrRgnProcPtr PolyProcPtr;
typedef PolyOrRgnProcPtr RgnProcPtr;
typedef pascal void (*BitsProcPtr)(BitMap *, Rect *, Rect *, short, RgnHandle);
typedef pascal short (*TxMeasProcPtr)(short, Ptr, Point *, Point *, FontInfo *);

// Like the standard QDProcs struct from Quickdraw.h but with better types.
typedef struct qdprocs {
	TextProcPtr Text;
	LineProcPtr Line;
	RectProcPtr Rect;
	RRectProcPtr RRect;
	OvalProcPtr Oval;
	ArcProcPtr Arc;
	PolyProcPtr Poly;
	RgnProcPtr Rgn;
	BitsProcPtr Bits;
	ProcPtr Comment; // unused
	TxMeasProcPtr TxMeas;
	ProcPtr GetPic; // unused
	ProcPtr PutPic; // unused
} qdprocs;

static qdprocs g_std_qdprocs;
#ifndef USE_TRAP_PATCHING
static qdprocs g_qdprocs_2x;
#endif

// The filler field of the GrafPort seems like a nice place to stash our 2x flag
// except that bitsProc only receives a BitMap pointer, not a GrafPtr, and not
// all BitMaps are in GrafPorts. For now, only the main screen is considered 2x.

void set_port_2x(GrafPtr port) {
//	port->filler |= k_port_is_2x;
#ifndef USE_TRAP_PATCHING
	port->grafProcs = (QDProcs *)&g_qdprocs_2x;
#endif
}

static Boolean unset_port_2x(GrafPtr port) {
//	port->filler &= ~k_port_is_2x;
#ifndef USE_TRAP_PATCHING
	port->grafProcs = nil;
#endif
}

static Boolean is_bits_2x(BitMap *bits) {
	return bits->baseAddr == qd.screenBits.baseAddr;
}

static Boolean is_port_2x(GrafPtr port) {
//	return port->filler & k_port_is_2x \

	return is_bits_2x(&port->portBits) \
		&& nil == port->picSave \
		&& nil == port->rgnSave \
		&& nil == port->polySave;
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

static obj_handle new_obj_2x(obj_handle obj) {
	obj_handle obj_2x;
	obj_ptr obj_2x_ptr;

	obj_2x = obj;

	if (noErr == HandToHand((Handle *)&obj_2x)) {
		HLock((Handle)obj_2x);
		obj_2x_ptr = *obj_2x;
		double_shorts((short *)&obj_2x_ptr->bbox, (obj_2x_ptr->size - sizeof(short)) / sizeof(short));
		HUnlock((Handle)obj_2x);
	} else {
		obj_2x = nil;
	}

	return obj_2x;
}

static void dispose_obj_2x(obj_handle obj_2x) {
	DisposeHandle((Handle)obj_2x);
}

static pascal void Text_2x(short byte_count, Ptr text_buf, Point numer, Point denom) {
	TextProcPtr std_Text;
	GrafPtr port;
	PenState pen;

	start_accessing_globals();
	std_Text = g_std_qdprocs.Text;
	stop_accessing_globals();

	GetPort(&port);
	if (is_port_2x(port)) {
		// TODO: double spExtra
		// TODO: deduplicate pen size code
		double_point(&numer, &numer);
		GetPenState(&pen);
		Move(pen.pnLoc.h, pen.pnLoc.v);
		(std_Text)(byte_count, text_buf, numer, denom);
		GetPenState(&pen);
		MoveTo(pen.pnLoc.h >> 1, pen.pnLoc.v >> 1);
	} else {
		(std_Text)(byte_count, text_buf, numer, denom);
	}
}

static pascal void Line_2x(Point end_point) {
	LineProcPtr std_Line;
	GrafPtr port;
	Point end_point_2x;
	PenState pen;

	start_accessing_globals();
	std_Line = g_std_qdprocs.Line;
	stop_accessing_globals();

	GetPort(&port);
	if (is_port_2x(port)) {
		double_point(&end_point, &end_point_2x);
		GetPenState(&pen);
		PenSize(pen.pnSize.h << 1, pen.pnSize.v << 1);
		Move(pen.pnLoc.h, pen.pnLoc.v);
		(std_Line)(end_point_2x);
		PenSize(pen.pnSize.h, pen.pnSize.v);
		MoveTo(end_point.h, end_point.v);
	} else {
		(std_Line)(end_point);
	}
}

static pascal void Rect_2x(GrafVerb verb, Rect rect) {
	RectProcPtr std_Rect;
	GrafPtr port;
	PenState pen;

	start_accessing_globals();
	std_Rect = g_std_qdprocs.Rect;
	stop_accessing_globals();

	GetPort(&port);
	if (is_port_2x(port)) {
		double_rect(&rect, &rect);
		GetPenState(&pen);
		PenSize(pen.pnSize.h << 1, pen.pnSize.v << 1);
		(std_Rect)(verb, rect);
		PenSize(pen.pnSize.h, pen.pnSize.v);
	} else {
		(std_Rect)(verb, rect);
	}
}

static pascal void RRect_2x(GrafVerb verb, Rect rect, short oval_width, short oval_height) {
	RRectProcPtr std_RRect;
	GrafPtr port;
	PenState pen;

	start_accessing_globals();
	std_RRect = g_std_qdprocs.RRect;
	stop_accessing_globals();

	GetPort(&port);
	if (is_port_2x(port)) {
		double_rect(&rect, &rect);
		GetPenState(&pen);
		PenSize(pen.pnSize.h << 1, pen.pnSize.v << 1);
		(std_RRect)(verb, rect, oval_width << 1, oval_height << 1);
		PenSize(pen.pnSize.h, pen.pnSize.v);
	} else {
		(std_RRect)(verb, rect, oval_width, oval_height);
	}
}

static pascal void Oval_2x(GrafVerb verb, Rect rect) {
	OvalProcPtr std_Oval;
	GrafPtr port;
	PenState pen;

	start_accessing_globals();
	std_Oval = g_std_qdprocs.Oval;
	stop_accessing_globals();

	GetPort(&port);
	if (is_port_2x(port)) {
		double_rect(&rect, &rect);
		GetPenState(&pen);
		PenSize(pen.pnSize.h << 1, pen.pnSize.v << 1);
		(std_Oval)(verb, rect);
		PenSize(pen.pnSize.h, pen.pnSize.v);
	} else {
		(std_Oval)(verb, rect);
	}
}

static pascal void Arc_2x(GrafVerb verb, Rect rect, short start_angle, short arc_angle) {
	ArcProcPtr std_Arc;
	GrafPtr port;
	PenState pen;

	start_accessing_globals();
	std_Arc = g_std_qdprocs.Arc;
	stop_accessing_globals();

	GetPort(&port);
	if (is_port_2x(port)) {
		double_rect(&rect, &rect);
		GetPenState(&pen);
		PenSize(pen.pnSize.h << 1, pen.pnSize.v << 1);
		(std_Arc)(verb, rect, start_angle, arc_angle);
		PenSize(pen.pnSize.h, pen.pnSize.v);
	} else {
		(std_Arc)(verb, rect, start_angle, arc_angle);
	}
}

static pascal void Poly_2x(GrafVerb verb, obj_handle poly) {
	PolyProcPtr std_Poly;
	GrafPtr port;
	PenState pen;
	obj_handle poly_2x;

	start_accessing_globals();
	std_Poly = g_std_qdprocs.Poly;
	stop_accessing_globals();

	GetPort(&port);
	if (is_port_2x(port)) {
		poly_2x = new_obj_2x(poly);
		if (nil != poly_2x) {
			GetPenState(&pen);
			PenSize(pen.pnSize.h << 1, pen.pnSize.v << 1);
			(std_Poly)(verb, poly_2x);
			PenSize(pen.pnSize.h, pen.pnSize.v);
			dispose_obj_2x(poly_2x);
		}
	} else {
		(std_Poly)(verb, poly);
	}
}

static pascal void Rgn_2x(GrafVerb verb, obj_handle rgn) {
	RgnProcPtr std_Rgn;
	GrafPtr port;
	PenState pen;
	obj_handle rgn_2x;

	start_accessing_globals();
	std_Rgn = g_std_qdprocs.Rgn;
	stop_accessing_globals();

	GetPort(&port);
	if (is_port_2x(port)) {
		rgn_2x = new_obj_2x(rgn);
		if (nil != rgn_2x) {
			GetPenState(&pen);
			PenSize(pen.pnSize.h << 1, pen.pnSize.v << 1);
			(std_Rgn)(verb, rgn_2x);
			PenSize(pen.pnSize.h, pen.pnSize.v);
			dispose_obj_2x(rgn_2x);
		}
	} else {
		(std_Rgn)(verb, rgn);
	}
}

static pascal void Bits_2x(BitMap *src_bits, Rect *src_rect, Rect *dst_rect, short mode, RgnHandle mask_rgn) {
	BitsProcPtr std_Bits;
	GrafPtr port;
	Rect new_src_rect;
	Rect new_dst_rect;

	start_accessing_globals();
	std_Bits = g_std_qdprocs.Bits;
	stop_accessing_globals();

	GetPort(&port);
	if (is_bits_2x(src_bits)) {
		double_rect(src_rect, &new_src_rect);
	} else {
		new_src_rect = *src_rect;
	}

	if (is_port_2x(port)) {
		double_rect(dst_rect, &new_dst_rect);
	} else {
		new_dst_rect = *dst_rect;
	}

	(std_Bits)(src_bits, &new_src_rect, &new_dst_rect, mode, mask_rgn);
}

static pascal short TxMeas_2x(short byte_count, Ptr text_buf, Point *numer, Point *denom, FontInfo *info) {
	TxMeasProcPtr std_TxMeas;
	GrafPtr port;
	short width;

	start_accessing_globals();
	std_TxMeas = g_std_qdprocs.TxMeas;
	stop_accessing_globals();

	GetPort(&port);
	if (is_port_2x(port)) {
		double_point(numer, numer);
		width = (std_TxMeas)(byte_count, text_buf, numer, denom, info);
		half_point(numer, numer);
	} else {
		width = (std_TxMeas)(byte_count, text_buf, numer, denom, info);
	}

	return width;
}

static ProcPtr set_toolbox_trap(ProcPtr new_proc, short trap) {
	ProcPtr old_proc;

	old_proc = (ProcPtr)NGetTrapAddress(trap, ToolTrap);
	NSetTrapAddress((long)new_proc, trap, ToolTrap);
	return old_proc;
}

#define patch_qdproc(name) \
	g_std_qdprocs.name = set_toolbox_trap(name##_2x, _Std##name)

#define unpatch_qdproc(name) \
	set_toolbox_trap(g_std_qdprocs.name, _Std##name)

void init_qdprocs(void) {
#ifdef USE_TRAP_PATCHING
	remember_globals();
	patch_qdproc(Text);
	patch_qdproc(Line);
	patch_qdproc(Rect);
	patch_qdproc(RRect);
	patch_qdproc(Oval);
	patch_qdproc(Arc);
	patch_qdproc(Poly);
	patch_qdproc(Rgn);
	patch_qdproc(Bits);
	patch_qdproc(TxMeas);
#else
	SetStdProcs((QDProcs *)&g_std_qdprocs);
	SetStdProcs((QDProcs *)&g_qdprocs_2x);
	g_qdprocs_2x.Text = &Text_2x;
	g_qdprocs_2x.Line = &Line_2x;
	g_qdprocs_2x.Rect = &Rect_2x;
	g_qdprocs_2x.RRect = &RRect_2x;
	g_qdprocs_2x.Oval = &Oval_2x;
	g_qdprocs_2x.Arc = &Arc_2x;
	g_qdprocs_2x.Poly = &Poly_2x;
	g_qdprocs_2x.Rgn = &Rgn_2x;
	g_qdprocs_2x.Bits = &Bits_2x;
	g_qdprocs_2x.TxMeas = &TxMeas_2x;
#endif
}

void deinit_qdprocs(void) {
#ifdef USE_TRAP_PATCHING
	unpatch_qdproc(Text);
	unpatch_qdproc(Line);
	unpatch_qdproc(Rect);
	unpatch_qdproc(RRect);
	unpatch_qdproc(Oval);
	unpatch_qdproc(Arc);
	unpatch_qdproc(Poly);
	unpatch_qdproc(Rgn);
	unpatch_qdproc(Bits);
	unpatch_qdproc(TxMeas);
#endif
}

// TODO: handle origin
// TODO: don't double to more than 32766
// TODO: handle cliprgn
