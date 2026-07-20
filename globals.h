/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#ifndef HIDPI_GLOBALS_H
#define HIDPI_GLOBALS_H

#include "constants.h"
#include "typedefs.h"

extern Point MTemp : 0x828;
extern Point RawMouse : 0x82C;
extern Point Mouse : 0x830;
//extern Rect CrsrPin : 0x834; // in <SysEqu.h>
extern Rect CrsrRect : 0x83C;
extern Cursor TheCrsr : 0x844;
extern Ptr CrsrAddr : 0x888;
extern Ptr CrsrSave : 0x88C;
extern short CrsrRow : 0x8AC;
extern Boolean CrsrVis : 0x8CC;
extern Boolean CrsrBusy : 0x8CD;
extern Boolean CrsrNew : 0x8CE;
extern Boolean CrsrCouple : 0x8CF;
extern short CrsrState : 0x8D0;
extern char CrsrObscure : 0x8D2;
extern Rect Scratch8 : 0x9FA;

extern void_proc_ptr JHideCursor : 0x800;
extern void_proc_ptr JShowCursor : 0x804;
extern JShieldCursor_proc_ptr JShieldCursor : 0x808;
extern JScrnSize_proc_ptr JScrnSize : 0x810;
extern void_proc_ptr JInitCrsr : 0x814;
extern JSetCursor_proc_ptr JSetCrsr : 0x818;
extern void_proc_ptr JCrsrObscure : 0x81C;
extern JSetCCursor_proc_ptr JSetCCrsr : 0x890;

enum
{
	t_none,
	t_PointPtr,
	t_PolyHandle,
	t_RectPtr,
	t_RgnHandle
};

typedef struct
{
	short type;
	void *item;
	void *orig_item;
}
debigulation_t;

#define k_max_debigulations 4

typedef struct
{
	app_data_h app_data;
	app_data_t system_data;
	app_data_p system_data_p;

	// Temporary data for the big version of the region being defined by OpenRgn/CloseRgn.
	rgntmp_t rgntmp_big;

	// The rgnset of the region being closed by CloseRgn.
	rgnset_h closed_rgnset;

	// The temporary buffer for the region being closed by CloseRgn.
	Handle closed_rgnbuf;

	// 2x replacement for TheCrsr.hotSpot.
	Point hotspot_2x;

	// 2x replacement for TheCrsr.data.
	long data_2x[k_cursor_longs_2x];

	// 2x replacement for TheCrsr.mask.
	long mask_2x[k_cursor_longs_2x];

	// 2x replacement for CrsrSave.
	long save_2x[k_cursor_save_longs_2x];

	debigulation_t debigulations[k_max_debigulations];
	short num_debigulations;
	short debigulate_crsrstate;

	Boolean cursor_changed;
}
data_t;

/*
typedef struct
{
	short offset;
	short ignore;
}
offset_t;

typedef union
{
	offset_t offset;
	long old_address;
}
patch_value_t;
*/

// The layout of this struct must match each *_patch procedure, which
// achieve this layout by starting with the "declare" macro.
// Some functions have arguments and therefore have an added 4-byte LINK
// instruction.
typedef struct
{
	short bra;
	short old_address[];
}
patch_proc_t;

// The size and layout of this struct must match the patch table in patch_table.c.
// The first short in the table is ignored and overwritten by install.
typedef struct
{
	short old_address_index;
	short routine;
//	patch_value_t value;
	short offset;
}
patch_t;

typedef struct
{
	long bsr_gp;
	Ptr gp;
	long bsr_pt;
	// The size of the struct elements up to here must match the size of the
	// code that precedes the patch table in patch_table.c.
	patch_t patches[];
}
code_t;

// This struct must match the order in which globals are declared in globals.c.
// To keep it simple and to minimize the amount of space used in the compiled
// code for globals defaults, use as few globals as possible.
#if __option(a4_globals)
typedef struct
{
	data_t *data;
}
globals_t;
#endif

extern data_t *g_data;

// In hidpi.c.
#if !__option(a4_globals)
extern Boolean g_installed;
#endif

// In patch_table.c.
#if !__option(a4_globals)
void link_globals(void);
#endif
void begin_globals(long *token);
void end_globals(long token);

#endif
