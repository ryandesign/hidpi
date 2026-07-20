/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#ifndef HIDPI_TYPEDEFS_H
#define HIDPI_TYPEDEFS_H

typedef pascal void (*void_proc_ptr)(void);
typedef pascal void (*JScrnSize_proc_ptr)(short *, short *);
typedef pascal void (*JSetCCursor_proc_ptr)(CCrsrHandle);
typedef pascal void (*JSetCursor_proc_ptr)(Point, short, Ptr, Ptr);
typedef pascal void (*JShieldCursor_proc_ptr)(short, short, short, short);
typedef pascal void (*ScrnBitMap_proc_ptr)(BitMap *);

typedef struct
{
	long bookkeeping; // rgnMax, rgnIndex
	Handle buf; // rgnBuf
}
rgntmp_t;

typedef struct
{
	short QDSpareD;
	short QDSpareC;
	short QDSpareB;
	short QDSpareA;
	short QDSpare9;
	short QDSpare8;
	short QDSpare7;
	short QDSpare6;
	short QDSpare5;
	short QDSpare4;
	short QDSpare3;
	long playIndex;
	FMOutput *fontPtr;
	Fixed fixTxWid;
	Point patAlign;
	short polyMax;
	PolyHandle thePoly;
	short QDSpare0;
	long playPic;
	rgntmp_t rgntmp;
	Region wideData;
	RgnPtr wideMaster;
	RgnHandle wideOpen;
	long randSeed;
	BitMap screenBits;
	Cursor arrow;
	Pattern dkGray;
	Pattern ltGray;
	Pattern gray;
	Pattern white;
	Pattern black;
	GrafPtr thePort;
}
qd_globals_t;

typedef struct rgnset_t rgnset_t;
typedef rgnset_t *rgnset_p;
typedef rgnset_p *rgnset_h;
struct rgnset_t
{
	rgnset_h next;
	RgnHandle original;
	RgnHandle copy;
	RgnHandle big;
};

typedef struct app_data_t app_data_t;
typedef app_data_t *app_data_p;
typedef app_data_p *app_data_h;
struct app_data_t
{
	app_data_h next;
	THz zone;
	// Linked list of rgnsets mapping original regions to their big versions.
	rgnset_h rgnsets;
};

#endif
