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

#ifdef USE_TRAP_PATCHING
// Pointers to the original versions of all of our patched traps.
typedef struct trap_procs {
#else
// Like the standard QDProcs struct from Quickdraw.h but with better types.
typedef struct qd_procs {
#endif
	TextProcPtr StdText;
	LineProcPtr StdLine;
	RectProcPtr StdRect;
	RRectProcPtr StdRRect;
	OvalProcPtr StdOval;
	ArcProcPtr StdArc;
	PolyProcPtr StdPoly;
	RgnProcPtr StdRgn;
	BitsProcPtr StdBits;
#ifndef USE_TRAP_PATCHING
	ProcPtr StdComment; // unused
#endif
	TxMeasProcPtr StdTxMeas;
#ifndef USE_TRAP_PATCHING
	ProcPtr StdGetPic; // unused
	ProcPtr StdPutPic; // unused
} qd_procs;
#else
} trap_procs;
#endif

#ifdef USE_TRAP_PATCHING
static trap_procs g_procs_lo;
#else
static qd_procs g_procs_lo;
static qd_procs g_procs_hi;
#endif

// The filler field of the GrafPort seems like a nice place to stash our 2x flag
// except that bitsProc only receives a BitMap pointer, not a GrafPtr, and not
// all BitMaps are in GrafPorts. For now, only the main screen is considered 2x.

void set_port_2x(GrafPtr port) {
//	port->filler |= k_port_is_2x;
#ifndef USE_TRAP_PATCHING
	port->grafProcs = (QDProcs *)&g_procs_hi;
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

static void double_shorts(short *src_buf, short *dst_buf, short size) {
	short count;
	short src;
	int i;

	count = size / sizeof(short);
	for (i = 0; i < count; ++i) {
		src = src_buf[i];
		if (src < 0) {
			if (src >= -16384) {
				dst_buf[i] = src << 1;
			} else {
				dst_buf[i] = -32768;
			}
		} else {
			if (src < 16384) {
				dst_buf[i] = src << 1;
			} else if (32767 == src) {
				dst_buf[i] = 32767;
			} else {
				dst_buf[i] = 32766;
			}
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
	short size;
	char state;

	size = (**obj).size;
	obj_2x = (obj_handle)NewHandle(size);

	if (nil != obj_2x) {
		state = HGetState((Handle)obj);
		HLock((Handle)obj);
		HLock((Handle)obj_2x);
		(**obj_2x).size = size;
		double_shorts((short *)&(**obj).bbox, (short *)&(**obj_2x).bbox, size - sizeof(short));
		HUnlock((Handle)obj_2x);
		HSetState((Handle)obj, state);
	} else {
		obj_2x = nil;
	}

	return obj_2x;
}

static void dispose_obj_2x(obj_handle obj_2x) {
	DisposeHandle((Handle)obj_2x);
}

typedef struct port_state {
	GrafPtr port;
	Rect bits_bounds;
	RgnHandle clip;
	RgnHandle vis;
	PenState pen;
} port_state;

static void begin_2x(port_state *state, GrafPtr port, Boolean pen_size_2x) {
	RgnHandle clip;
	RgnHandle clip_2x;
	RgnHandle vis;
	RgnHandle vis_2x;

#ifdef USE_TRAP_PATCHING
	state->port = port;

	state->bits_bounds = port->portBits.bounds;
	double_rect(&port->portBits.bounds, &port->portBits.bounds);

	clip = NewRgn();
	if (nil != clip) {
		GetClip(clip);
		clip_2x = (RgnHandle)new_obj_2x((obj_handle)clip);
		if (nil != clip_2x) {
			SetClip(clip_2x);
			dispose_obj_2x((obj_handle)clip_2x);
		} else {
			DisposeRgn(clip);
			clip = nil;
		}
	}
	state->clip = clip;

	vis = port->visRgn;
	vis_2x = (RgnHandle)new_obj_2x((obj_handle)vis);
	if (nil != vis_2x) {
		port->visRgn = vis_2x;
	} else {
		vis = nil;
	}
	state->vis = vis;
#endif

	GetPenState(&state->pen);
	if (pen_size_2x) {
		PenSize(state->pen.pnSize.h << 1, state->pen.pnSize.v << 1);
	} else {
		*(long *)&state->pen.pnSize = 0xFFFFFFFF;
	}
}

static void end_2x(port_state *state) {
#ifdef USE_TRAP_PATCHING
	if (nil != state->port) {
		state->port->portBits.bounds = state->bits_bounds;
	}

	if (nil != state->clip) {
		SetClip(state->clip);
		DisposeRgn(state->clip);
	}

	if (nil != state->vis) {
		dispose_obj_2x((obj_handle)state->port->visRgn);
		state->port->visRgn = state->vis;
	}
#endif

	if (0xFFFFFFFF != *(long *)&state->pen.pnSize) {
		PenSize(state->pen.pnSize.h, state->pen.pnSize.v);
	}
}

static pascal void StdText_hi(short byte_count, Ptr text_buf, Point numer, Point denom) {
	port_state state;
	TextProcPtr proc;
	GrafPtr port;

	start_accessing_globals();
	proc = g_procs_lo.StdText;
	stop_accessing_globals();

	GetPort(&port);
	if (is_port_2x(port)) {
		begin_2x(&state, port, false);
		double_point(&numer, &numer);
		Move(state.pen.pnLoc.h, state.pen.pnLoc.v);
		(proc)(byte_count, text_buf, numer, denom);
		end_2x(&state);
		GetPenState(&state.pen);
		MoveTo(state.pen.pnLoc.h >> 1, state.pen.pnLoc.v >> 1);
	} else {
		(proc)(byte_count, text_buf, numer, denom);
	}
}

static pascal void StdLine_hi(Point end_point) {
	port_state state;
	LineProcPtr proc;
	GrafPtr port;
	Point end_point_2x;

	start_accessing_globals();
	proc = g_procs_lo.StdLine;
	stop_accessing_globals();

	GetPort(&port);
	if (is_port_2x(port)) {
		begin_2x(&state, port, true);
		double_point(&end_point, &end_point_2x);
		Move(state.pen.pnLoc.h, state.pen.pnLoc.v);
		(proc)(end_point_2x);
		MoveTo(end_point.h, end_point.v);
		end_2x(&state);
	} else {
		(proc)(end_point);
	}
}

static pascal void StdRectOrOval_hi(RectOrOvalProcPtr proc, GrafVerb verb, Rect rect) {
	port_state state;
	GrafPtr port;

	GetPort(&port);
	if (is_port_2x(port)) {
		begin_2x(&state, port, true);
		double_rect(&rect, &rect);
		(proc)(verb, rect);
		end_2x(&state);
	} else {
		(proc)(verb, rect);
	}
}

static pascal void StdRect_hi(GrafVerb verb, Rect rect) {
	RectProcPtr proc;

	start_accessing_globals();
	proc = g_procs_lo.StdRect;
	stop_accessing_globals();

	StdRectOrOval_hi(proc, verb, rect);
}

static pascal void StdOval_hi(GrafVerb verb, Rect rect) {
	OvalProcPtr proc;

	start_accessing_globals();
	proc = g_procs_lo.StdOval;
	stop_accessing_globals();

	StdRectOrOval_hi(proc, verb, rect);
}

static pascal void StdRRectOrArc_hi(RRectOrArcProcPtr proc, Boolean shorts_2x, GrafVerb verb, Rect rect, short short1, short short2) {
	port_state state;
	GrafPtr port;

	GetPort(&port);
	if (is_port_2x(port)) {
		begin_2x(&state, port, true);
		double_rect(&rect, &rect);
		if (shorts_2x) {
			short1 <<= 1;
			short2 <<= 1;
		}
		(proc)(verb, rect, short1, short2);
		end_2x(&state);
	} else {
		(proc)(verb, rect, short1, short2);
	}
}

static pascal void StdRRect_hi(GrafVerb verb, Rect rect, short oval_width, short oval_height) {
	RRectProcPtr proc;

	start_accessing_globals();
	proc = g_procs_lo.StdRRect;
	stop_accessing_globals();

	StdRRectOrArc_hi(proc, true, verb, rect, oval_width, oval_height);
}

static pascal void StdArc_hi(GrafVerb verb, Rect rect, short start_angle, short arc_angle) {
	ArcProcPtr proc;

	start_accessing_globals();
	proc = g_procs_lo.StdArc;
	stop_accessing_globals();

	StdRRectOrArc_hi(proc, false, verb, rect, start_angle, arc_angle);
}

static pascal void StdPolyOrRgn_hi(PolyOrRgnProcPtr proc, GrafVerb verb, obj_handle obj) {
	port_state state;
	GrafPtr port;
	obj_handle obj_2x;

	GetPort(&port);
	if (is_port_2x(port)) {
		obj_2x = new_obj_2x(obj);
		if (nil != obj_2x) {
			begin_2x(&state, port, true);
			(proc)(verb, obj_2x);
			end_2x(&state);
			dispose_obj_2x(obj_2x);
		}
	} else {
		(proc)(verb, obj);
	}
}

static pascal void StdPoly_hi(GrafVerb verb, obj_handle poly) {
	PolyProcPtr proc;

	start_accessing_globals();
	proc = g_procs_lo.StdPoly;
	stop_accessing_globals();

	StdPolyOrRgn_hi(proc, verb, poly);
}

static pascal void StdRgn_hi(GrafVerb verb, obj_handle rgn) {
	RgnProcPtr proc;

	start_accessing_globals();
	proc = g_procs_lo.StdRgn;
	stop_accessing_globals();

	StdPolyOrRgn_hi(proc, verb, rgn);
}

static pascal void StdBits_hi(BitMap *src_bits, Rect *src_rect, Rect *dst_rect, short mode, RgnHandle mask_rgn) {
	port_state state;
	BitsProcPtr proc;
	GrafPtr port;
	Rect new_src_rect;
	Rect new_dst_rect;

	start_accessing_globals();
	proc = g_procs_lo.StdBits;
	stop_accessing_globals();

	GetPort(&port);
	if (is_bits_2x(src_bits)) {
		double_rect(src_rect, &new_src_rect);
	} else {
		new_src_rect = *src_rect;
	}

	if (is_port_2x(port)) {
		begin_2x(&state, port, false);
		double_rect(dst_rect, &new_dst_rect);
	} else {
		new_dst_rect = *dst_rect;
	}

	(proc)(src_bits, &new_src_rect, &new_dst_rect, mode, mask_rgn);

	if (is_port_2x(port)) {
		end_2x(&state);
	}
}

static pascal short StdTxMeas_hi(short byte_count, Ptr text_buf, Point *numer, Point *denom, FontInfo *info) {
	TxMeasProcPtr proc;
	GrafPtr port;
	short width;

	start_accessing_globals();
	proc = g_procs_lo.StdTxMeas;
	stop_accessing_globals();

	GetPort(&port);
	if (is_port_2x(port)) {
		double_point(numer, numer);
		width = (proc)(byte_count, text_buf, numer, denom, info);
		half_point(numer, numer);
	} else {
		width = (proc)(byte_count, text_buf, numer, denom, info);
	}

	return width;
}

static ProcPtr set_toolbox_trap(ProcPtr new_proc, short trap, TrapType type) {
	ProcPtr old_proc;

	old_proc = (ProcPtr)NGetTrapAddress(trap, type);
	NSetTrapAddress((long)new_proc, trap, type);
	return old_proc;
}

#define patch_trap(name) \
	g_procs_lo.name = set_toolbox_trap(name##_hi, \
		_##name, _##name & 0x0800 ? ToolTrap : OSTrap)

#define unpatch_trap(name) \
	set_toolbox_trap(g_procs_lo.name, \
		_##name, _##name & 0x0800 ? ToolTrap : OSTrap)

void init_qdprocs(void) {
#ifdef USE_TRAP_PATCHING
	remember_globals();
	patch_trap(StdText);
	patch_trap(StdLine);
	patch_trap(StdRect);
	patch_trap(StdRRect);
	patch_trap(StdOval);
	patch_trap(StdArc);
	patch_trap(StdPoly);
	patch_trap(StdRgn);
	patch_trap(StdBits);
	patch_trap(StdTxMeas);
#else
	SetStdProcs((QDProcs *)&g_procs_lo);
	SetStdProcs((QDProcs *)&g_procs_hi);
	g_procs_hi.StdText = &StdText_hi;
	g_procs_hi.StdLine = &StdLine_hi;
	g_procs_hi.StdRect = &StdRect_hi;
	g_procs_hi.StdRRect = &StdRRect_hi;
	g_procs_hi.StdOval = &StdOval_hi;
	g_procs_hi.StdArc = &StdArc_hi;
	g_procs_hi.StdPoly = &StdPoly_hi;
	g_procs_hi.StdRgn = &StdRgn_hi;
	g_procs_hi.StdBits = &StdBits_hi;
	g_procs_hi.StdTxMeas = &StdTxMeas_hi;
#endif
}

void deinit_qdprocs(void) {
#ifdef USE_TRAP_PATCHING
	unpatch_trap(StdText);
	unpatch_trap(StdLine);
	unpatch_trap(StdRect);
	unpatch_trap(StdRRect);
	unpatch_trap(StdOval);
	unpatch_trap(StdArc);
	unpatch_trap(StdPoly);
	unpatch_trap(StdRgn);
	unpatch_trap(StdBits);
	unpatch_trap(StdTxMeas);
#endif
}

// TODO: handle origin
// TODO: handle cliprgn
