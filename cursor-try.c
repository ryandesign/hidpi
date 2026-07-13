/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#include "cursor_stuff.h"

#include <Traps.h>
#include <Values.h>

#include "globals.h"
#include "install.h"
#include "init_data.h"
#include "JHideCursor_patch.h"
#include "JInitCrsr_patch.h"
#include "JShowCursor_patch.h"
#include "cursor_stuff.h"
#include "macros.h"
#include "missing_traps.h"
#include "patch_table.h"
#include "uninstall.h"

//#include "SetUpA5.h"

// TODO: generate 68020 code in functions only reached on color quickdraw.
// "#pragma options(mc68020)" takes effect for the entire function!

/*
void_proc_ptr g_JHideCursor;
void_proc_ptr g_JShowCursor;
JShieldCursor_proc_ptr g_JShieldCursor;
void_proc_ptr g_JInitCrsr;
JSetCursor_proc_ptr g_JSetCrsr;
void_proc_ptr g_JCrsrObscure;
JSetCCursor_proc_ptr g_JSetCCursor;

void_proc_ptr g_SystemTask;
*/

//Boolean g_cursor_changed = false;

Boolean g_has_color_quickdraw;

// TODO: delete
static void pixdub(Ptr src, Ptr dst, short src_rowbytes, short dst_rowbytes, short height)
{
	long chunk_2x = 0;
	short chunk_1x;
	short x;
	short y;
#if k_scale >= 4
	short band;
#endif

	do
	{
		chunk_1x = *(short *)src;
		src += src_rowbytes;

#if k_scale >= 4
		for (band = k_scale / 2 - 1; band >= 0; --band)
		{
#endif
			x = 16 * sizeof chunk_1x / k_scale;
			do
			{
				chunk_2x <<= k_scale;
				if (chunk_1x < 0) chunk_2x |= (1 << k_scale) - 1;
				chunk_1x <<= 1;
			}
			while (--x);

			y = k_scale;
			do
			{
				*(long *)dst = chunk_2x;
				dst += dst_rowbytes;
			}
			while (--y);

#if k_scale >= 4
			if (band > 0)
			{
				dst -= k_scale * dst_rowbytes - sizeof chunk_2x;
			}
			else
			{
				dst -= (k_scale / 2 - 1) * sizeof chunk_2x;
			}
		}
#endif
//return;
	}
	while (--height);
}

// TODO delete
static void invert_cursor_rect(void)
{
	register Ptr addr;
	register short y;
	register short row_advance = ScreenRow /*- (k_scale - 1)*/;

	CrsrBusy = true;
	addr = CrsrAddr;

	for (y = rect_height(&CrsrRect); y > 0; --y)
	{
		*(long *)addr = ~*(long *)addr;
		addr += row_advance;
	}

	CrsrBusy = false;
}

// TODO delete
static void outline_cursor_rect(void)
{
	register Ptr addr;
	register long row;
	register short y, height;
	register short row_advance = ScreenRow /*- (k_scale - 1)*/;

	CrsrBusy = true;
	height = rect_height(&CrsrRect);
	addr = CrsrAddr;

	if (height >= 1)
	{
		*(long *)addr = ~*(long *)addr;
		addr += row_advance;
	}

	for (y = height - 2; y > 0; --y)
	{
		row = *(long *)addr;
		*(long *)addr = row ^ 0x80000001;
		addr += row_advance;
	}

	if (height >= 2)
	{
		*(long *)addr = ~*(long *)addr;
	}

	CrsrBusy = false;
}

#if 0
static pascal void jset_ccursor_2x(CCrsrHandle cursor)
{
#pragma options(mc68020)

	jset_ccursor_proc_ptr old_jset_ccursor;

	SetUpA5();
	old_jset_ccursor = g_JSetCCursor;
	RestoreA5();

	// TODO
	old_jset_ccursor(cursor);
}
#endif

static void event_loop(void)
{
	Boolean done = false;
	short cursor_id = 0;
	EventRecord event;

	while (!done)
	{
#if 1
		WaitNextEvent(everyEvent, &event, MAXLONG, nil);
#else
		GetNextEvent(everyEvent, &event);
		SystemTask();
#endif
		switch (event.what)
		{
			case mouseDown:
				if (cursor_id >= 4)
				{
					cursor_id = 0;
					SetCursor(&qd.arrow);
				}
				else
				{
					CursHandle cursor_handle;

					++cursor_id;
					cursor_handle = GetCursor(cursor_id);
					if (nil != cursor_handle)
					{
						SetCursor(*cursor_handle);
					}
				}
				break;
			case keyDown:
			case autoKey:
				done = true;
		}
	}
}

void main(void)
{
	MaxApplZone();
	InitGraf((Ptr)&qd.thePort);
	InitFonts();
	InitWindows();
	InitMenus();
	TEInit();
	InitDialogs(nil);

	{
		register MenuHandle menu;
	
		menu = NewMenu(128, "\p");
		InsertMenu(menu, 0);
		menu = NewMenu(129, "\pFile");
		InsertMenu(menu, 0);
		menu = NewMenu(130, "\pEdit");
		InsertMenu(menu, 0);
		menu = NewMenu(131, "\pLarry");
		InsertMenu(menu, 0);
		menu = NewMenu(132, "\pJohn");
		InsertMenu(menu, 0);
		menu = NewMenu(133, "\pSteve");
		InsertMenu(menu, 0);
		menu = NewMenu(134, "\pBruce");
		InsertMenu(menu, 0);
		DrawMenuBar();
	}

	{
		SysEnvRec env;
	
		SysEnvirons(curSysEnvVers, &env);
		g_has_color_quickdraw = env.hasColorQD;
	}

	link_globals();

	g_data = (data_t *)NewPtrClear(sizeof *g_data);
	require(g_data, NewPtrClear);
	require(init_data(g_data), init_data);

#if 0
	CrsrBusy = true; // TODO: check if necessary
	{
		Point hotspot = TheCrsr.hotSpot;

		pin_hotspot(&hotspot);
		*(long *)&g_data->hotspot_2x = *(long *)&hotspot * k_scale;
	}
	// TODO: Color QuickDraw
	BlockMoveData(CrsrSave, g_data->save_2x, sizeof(long) * rect_height(&CrsrRect));
	install_cursor_patches();
	CrsrBusy = false; // TODO: check if necessary
#endif

	{
		patch_t *patches = get_patch_table();
		require(install(patches), install);
	
		InitCursor();
	
		FlushEvents(everyEvent, 0);
	
		event_loop();
	
		uninstall(patches);
	}

install:
init_data:
NewPtrClear:
	FlushEvents(everyEvent, 0);
}
