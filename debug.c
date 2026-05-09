/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#include "debug.h"

#define k_debug_event_width 128
#define k_line_height 16

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
