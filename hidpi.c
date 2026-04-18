/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#include "qdprocs.h"

#include <Traps.h>
#include <Values.h>

#define k_beep_duration 4
#define k_in_front ((WindowPtr)-1)
#define k_menu_hilite_ticks 6L
#define k_scrollbar_adjust 15
#define k_whole_menu 0

#define r_mbar 128

#define r_MENU_apple 128

#define r_MENU_file 129
#define i_new 1
#define i_new_2x 2
#define i_close 3
#define i_quit 5

#define r_MENU_edit 130
#define i_undo 1
#define i_cut 3
#define i_copy 4
#define i_paste 5
#define i_clear 6

#define r_MENU_debug 131
#define i_syserr 1

#define r_STRx 128

#define r_WIND 128
#define k_obj_margin 8
#define k_obj_size 32
#define k_min_doc_width (k_obj_margin + 4 * (k_obj_size + k_obj_margin) + k_scrollbar_adjust)
#define k_min_doc_height (k_obj_margin + 3 * (k_obj_size + k_obj_margin) + k_scrollbar_adjust)
#define k_max_doc_width MAXSHORT
#define k_max_doc_height MAXSHORT

#define has_128k_rom() (ROM85 >= 0)

#define rect_width(rect) ((rect).right - (rect).left)
#define rect_height(rect) ((rect).bottom - (rect).top)

Boolean g_done = false;
Boolean g_has_color_quickdraw;
Boolean g_has_WaitNextEvent;
unsigned long g_sleep = MAXLONG;

static Boolean is_app_window(WindowPtr window) {
	Boolean result;

	if (nil == window) {
		result = false;
	} else {
		result = userKind == ((WindowPeek)window)->windowKind;
	}

	return result;
}

static Boolean is_da_window(WindowPtr window) {
	Boolean result;

	if (nil == window) {
		result = false;
	} else {
		result = ((WindowPeek)window)->windowKind < 0;
	}

	return result;
}

static void do_quit(void) {
	g_done = true;
}

static void do_close_window(WindowPtr window) {
	if (is_app_window(window)) {
		DisposeWindow(window);
	} else if (is_da_window(window)) {
		CloseDeskAcc(((WindowPeek)window)->windowKind);
	}
}

static void do_new_window(Boolean use_my_qdprocs) {
	WindowPtr window;
	Rect rect;
	GrafPtr saved_port;

	window = GetNewWindow(r_WIND, nil, k_in_front);
	if (window) {
		if (use_my_qdprocs) {
			window->grafProcs = &g_my_qdprocs;
			rect = window->portRect;
			SizeWindow(window, rect_width(rect) << 1, rect_height(rect) << 1, false);
		}
		GetPort(&saved_port);
		SetPort(window);
		ShowWindow(window);
		SetPort(saved_port);
	}
}

static void do_debug_menu(short menu_id, short menu_item) {
	switch (menu_item) {
		case i_syserr:
			SysError(dsZeroDivErr);
			break;
	}
}

static void do_edit_menu(short menu_id, short menu_item) {
	if (!SystemEdit(menu_item - 1)) {
		switch (menu_item) {
			case i_undo:
			case i_cut:
			case i_copy:
			case i_paste:
			case i_clear:
				break;
		}
	}
}

static void do_file_menu(short menu_id, short menu_item) {
	switch (menu_item) {
		case i_new:
		case i_new_2x:
			do_new_window(i_new_2x == menu_item);
			break;
		case i_close:
			do_close_window(FrontWindow());
			break;
		case i_quit:
			do_quit();
			break;
	}
}

static void do_apple_menu(short menu_id, short menu_item) {
	MenuHandle menu;
	GrafPtr saved_port;
	Str255 da_name;

	menu = GetMHandle(menu_id);
	if (nil != menu) {
		GetItem(GetMHandle(menu_id), menu_item, da_name);
		GetPort(&saved_port);
		OpenDeskAcc(da_name);
		SetPort(saved_port);
	}
}

static void do_menu(long menu_result) {
	long end_ticks;
	short menu_id;
	short menu_item;

	end_ticks = TickCount() + k_menu_hilite_ticks;
	menu_id = HiWord(menu_result);
	menu_item = LoWord(menu_result);

	switch (menu_id) {
		case r_MENU_apple:
			do_apple_menu(menu_id, menu_item);
			break;
		case r_MENU_file:
			do_file_menu(menu_id, menu_item);
			break;
		case r_MENU_edit:
			do_edit_menu(menu_id, menu_item);
			break;
		case r_MENU_debug:
			do_debug_menu(menu_id, menu_item);
			break;
	}

	while (TickCount() < end_ticks);
	HiliteMenu(0);
}

static void calc_content_rect(WindowPtr window, Rect *content_rect) {
	*content_rect = window->portRect;
	content_rect->right -= k_scrollbar_adjust;
	content_rect->bottom -= k_scrollbar_adjust;
}

static void calc_scrollbar_rgn(WindowPtr window, RgnHandle scrollbar_rgn) {
	RgnHandle content_rgn;
	Rect content_rect;

	if (nil != scrollbar_rgn) {
		content_rgn = NewRgn();
		if (nil != content_rgn) {
			calc_content_rect(window, &content_rect);
			RectRgn(content_rgn, &content_rect);
			RectRgn(scrollbar_rgn, &window->portRect);
			DiffRgn(scrollbar_rgn, content_rgn, scrollbar_rgn);
			DisposeRgn(content_rgn);
		}
	}
}

static void inval_scrollbar_rgn(WindowPtr window) {
	RgnHandle scrollbar_rgn;
	GrafPtr saved_port;

	scrollbar_rgn = NewRgn();
	if (nil != scrollbar_rgn) {
		calc_scrollbar_rgn(window, scrollbar_rgn);
		GetPort(&saved_port);
		SetPort(window);
		InvalRgn(scrollbar_rgn);
		SetPort(saved_port);
	}
}

static void activate_window(WindowPtr window, Boolean activate) {
	GrafPtr saved_port;

	if (is_app_window(window)) {
		inval_scrollbar_rgn(window);
	}
}

static void do_suspend_resume_event(EventRecord *event) {
	activate_window(FrontWindow(), event->message & resumeFlag);
}

static void do_mouse_moved_event(EventRecord *event) {
}

static void do_os_event(EventRecord *event) {
	switch ((event->message >> 24) & 0xFF) {
		case mouseMovedMessage:
			do_mouse_moved_event(event);
			break;
		case suspendResumeMessage:
			do_suspend_resume_event(event);
			break;
	}
}

static void do_disk_event(EventRecord *event) {
	Point top_left;

	if (noErr != HiWord(event->message)) {
		SetPt(&top_left, 98, 98);
		DILoad();
		DIBadMount(top_left, event->message);
		DIUnload();
	}
}

static void draw_app_window(WindowPtr window) {
	GrafPtr saved_port;
	Rect rect;
	RgnHandle saved_clip;
	FontInfo font_info;
	short width;
	Str255 str;
	PicHandle pic;
	short x;
	int i;
	Pattern pattern;
	PolyHandle poly;
	RgnHandle rgn;
	Handle icon;

	GetPort(&saved_port);
	SetPort(window);
	DrawGrowIcon(window);
	calc_content_rect(window, &rect);

	saved_clip = NewRgn();
	if (nil != saved_clip) {
		GetClip(saved_clip);
		ClipRect(&rect);
	}

	FillRect(&rect, qd.ltGray);

	TextFont(systemFont);
	TextFace(condense);
	GetFontInfo(&font_info);

	MoveTo(k_obj_margin, k_obj_margin + font_info.ascent);
	width = 0;
	for (i = 1; i <= 2; ++i) {
		GetIndString(str, r_STRx, i);
		DrawString(str);
		width += StringWidth(str);
	}
	Move(0, 2);
	Line(-width, 0);

	SetRect(&rect, 0, 0, k_obj_size, k_obj_size);

	pic = OpenPicture(&rect);

	OffsetRect(&rect, k_obj_margin + 3 * (k_obj_size + k_obj_margin), k_obj_margin);

	if (nil != pic) {
		MoveTo(0, 0);
		Line(k_obj_size - 1, k_obj_size - 1);
		Move(-(k_obj_size - 1), 0);
		Line(k_obj_size - 1, -(k_obj_size - 1));
		ClosePicture();
		DrawPicture(pic, &rect);
		KillPicture(pic);
	}

	OffsetRect(&rect, -3 * (k_obj_margin + k_obj_size), k_obj_margin + k_obj_size);

	for (x = rect.left, i = 1; x < rect.right; x += i + 2, ++i) {
		PenSize(i, 1);
		MoveTo(x, rect.top);
		LineTo(x, rect.bottom - 1);
	}
	PenSize(1, 1);

	BlockMove(&qd.gray, &pattern, sizeof(pattern));
	OffsetRect(&rect, k_obj_margin + k_obj_size, 0);

	FillRect(&rect, pattern);
	FrameRect(&rect);

	OffsetRect(&rect, k_obj_margin + k_obj_size, 0);

	FillRoundRect(&rect, k_obj_size >> 1, k_obj_size >> 1, pattern);
	FrameRoundRect(&rect, k_obj_size >> 1, k_obj_size >> 1);

	OffsetRect(&rect, k_obj_margin + k_obj_size, 0);

	FillOval(&rect, pattern);
	FrameOval(&rect);

	OffsetRect(&rect, -3 * (k_obj_margin + k_obj_size), k_obj_margin + k_obj_size);

	FillArc(&rect, 180, 225, pattern);
	FrameArc(&rect, 180, 225);

	OffsetRect(&rect, k_obj_margin + k_obj_size, 0);

	poly = OpenPoly();
	if (nil != poly) {
		MoveTo(rect.left, rect.bottom);
		LineTo(rect.left + (k_obj_size >> 1), rect.top);
		LineTo(rect.right, rect.bottom);
		ClosePoly();
		FillPoly(poly, pattern);
		FramePoly(poly);
		KillPoly(poly);
	}

	OffsetRect(&rect, k_obj_margin + k_obj_size, 0);

	rgn = NewRgn();
	if (nil != rgn) {
		OpenRgn();
		FrameRect(&rect);
		InsetRect(&rect, k_obj_size >> 2, k_obj_size >> 2);
		FrameOval(&rect);
		InsetRect(&rect, -(k_obj_size >> 2), -(k_obj_size >> 2));
		CloseRgn(rgn);
		FillRgn(rgn, pattern);
		FrameRgn(rgn);
		DisposeRgn(rgn);
	}

	OffsetRect(&rect, k_obj_margin + k_obj_size, 0);

	icon = GetIcon(noteIcon);
	if (nil != icon) {
		PlotIcon(&rect, icon);
	}

	if (nil != saved_clip) {
		SetClip(saved_clip);
		DisposeRgn(saved_clip);
	}

	SetPort(saved_port);
}

static void do_update_event(EventRecord *event) {
	WindowPtr window;

	window = (WindowPtr)event->message;
	if (nil != window) {
		BeginUpdate(window);
		if (is_app_window(window)) {
			draw_app_window(window);
		}
		EndUpdate(window);
	}
}

static void do_activate_event(EventRecord *event) {
	activate_window((WindowPtr)event->message, event->modifiers & activeFlag);
}

static void do_key_down_event(EventRecord *event) {
	long menu_result;

	if (event->modifiers & cmdKey) {
		menu_result = MenuKey(event->message & charCodeMask);
		do_menu(menu_result);
	}
}

static void do_grow_window(WindowPtr window, EventRecord *event) {
	long size;
	Rect rect;
	RgnHandle rgn;

	SetRect(&rect, k_min_doc_width, k_min_doc_height, k_max_doc_width, k_max_doc_height);
	size = GrowWindow(window, event->where, &rect);
	if (0 != size) {
		inval_scrollbar_rgn(window);
		SizeWindow(window, LoWord(size), HiWord(size), true);
		inval_scrollbar_rgn(window);
	}
}

static void do_mouse_down_event(EventRecord *event) {
	WindowPtr window;
	short part;

	part = FindWindow(event->where, &window);
	switch (part) {
		case inMenuBar:
			do_menu(MenuSelect(event->where));
			break;
		case inSysWindow:
			SystemClick(event, window);
			break;
		case inContent:
			if (FrontWindow() != window) {
				SelectWindow(window);
			}
			break;
		case inDrag:
			DragWindow(window, event->where, &screenBits.bounds);
			break;
		case inGrow:
			do_grow_window(window, event);
			break;
		case inGoAway:
			if (TrackGoAway(window, event->where)) {
				do_close_window(window);
			}
			break;
	}
}

static void do_event(EventRecord *event) {
	switch (event->what) {
		case mouseDown:
			do_mouse_down_event(event);
			break;
		case keyDown:
		case autoKey:
			do_key_down_event(event);
			break;
		case activateEvt:
			do_activate_event(event);
			break;
		case updateEvt:
			do_update_event(event);
			break;
		case diskEvt:
			do_disk_event(event);
			break;
		case osEvt:
			do_os_event(event);
			break;
	}
}

static Boolean is_menu_enabled(MenuHandle menu) {
	return (**menu).enableFlags & 1;
}

static void adjust_menus(void) {
	WindowPtr window;
	MenuHandle menu;
	Boolean enable;
	Boolean inval = false;

	window = FrontWindow();

	menu = GetMHandle(r_MENU_file);
	if (nil != menu) {
		if (nil == window) {
			DisableItem(menu, i_close);
		} else {
			EnableItem(menu, i_close);
		}
	}

	menu = GetMHandle(r_MENU_edit);
	if (nil != menu) {
		enable = is_da_window(window);
		if (enable != is_menu_enabled(menu)) {
			if (enable) {
				EnableItem(menu, k_whole_menu);
			} else {
				DisableItem(menu, k_whole_menu);
			}
			inval = true;
		}
	}

	if (inval) {
		DrawMenuBar();
	}
}

static void event_loop(void) {
	Boolean got_event;
	EventRecord event;

	while (!g_done) {
		adjust_menus();
		if (g_has_WaitNextEvent) {
			got_event = WaitNextEvent(everyEvent, &event, g_sleep, nil);
		} else {
			SystemTask();
			got_event = GetNextEvent(everyEvent, &event);
		}
		if (got_event) {
			do_event(&event);
		}
	}
}

static short get_num_toolbox_traps(void) {
	short num_traps;

	if (NGetTrapAddress(_InitGraf, ToolTrap) == NGetTrapAddress(0xAA6E, ToolTrap)) {
		num_traps = 0x200;
	} else {
		num_traps = 0x400;
	}

	return num_traps;
}

static TrapType get_trap_type(unsigned short trap) {
	TrapType trap_type;

	if (trap & 0x0800) {
		trap_type = ToolTrap;
	} else {
		trap_type = OSTrap;
	}

	return trap_type;
}

static Boolean has_trap(unsigned short trap) {
	TrapType trap_type;
	Boolean has_trap;

	if (has_128k_rom()) {
		trap_type = get_trap_type(trap);
		if (ToolTrap == trap_type) {
			trap &= 0x03FF;
			if (trap >= get_num_toolbox_traps()) {
				trap = _Unimplemented;
			}
		}
		has_trap = NGetTrapAddress(trap, trap_type) != NGetTrapAddress(_Unimplemented, ToolTrap);
	} else {
		has_trap = false;
	}

	return has_trap;
}

static Boolean init_app(void) {
	Boolean good = false;
	Handle menubar;
	MenuHandle menu;
	SysEnvRec env;

	menubar = GetNewMBar(r_mbar);
	if (nil == menubar) goto fail;
	SetMenuBar(menubar);
	DisposeHandle(menubar);

	menu = GetMHandle(r_MENU_apple);
	if (nil != menu) AddResMenu(menu, 'DRVR');

	SysEnvirons(curSysEnvVers, &env);
	// Tech Note TB 28: Problem with WaitNextEvent in MultiFinder 1.0
	if (env.systemVersion < 0x0500) g_sleep = 50;

	g_has_color_quickdraw = env.hasColorQD;
	g_has_WaitNextEvent = has_trap(_WaitNextEvent);

	good = true;

fail:
	return good;
}

// TODO: implement adjust_cursor
// TODO: add scroll bars
// TODO: add sample controls
// TODO: improve function names
// TODO: CrsrPin

static Boolean init(void) {
	Boolean good = false;

	if (!has_128k_rom()) goto fail;

	MaxApplZone();

	init_qdprocs();

	InitGraf((Ptr)&qd.thePort);
	InitFonts();
	InitWindows();
	InitMenus();
	TEInit();
	InitDialogs((ResumeProcPtr)NGetTrapAddress(_ExitToShell, ToolTrap));

	if (!init_app()) goto fail;

	InitCursor();
	FlushEvents(everyEvent & ~diskEvt, 0);
	good = true;
	goto done;

fail:
	SysBeep(k_beep_duration);

done:
	return good;
}

void main(void) {
	if (init()) {
		event_loop();
	}
}
