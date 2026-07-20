/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#include "rgnset.h"

#include "app_data.h"
#include "CopyRgn_patch.h"
#include "DisposeRgn_patch.h"
#include "embiggen.h"
#include "macros.h"
#include "typedefs.h"

void dispose_rgnset(RgnHandle original)
{
	app_data_h ah;
	app_data_p ap;
	rgnset_h last = nil;
	rgnset_h rh;
	rgnset_p rp;
	THz zone;
	THz old_zone;

	zone = HandleZone((Handle)original);

	ah = find_app_data(zone);
	ap = *ah;

	rh = ap->rgnsets;
	while (nil != rh)
	{
		rp = *rh;
		if (original == rp->original) break;
		last = rh;
		rh = rp->next;
	}
	require(rh, end);

	rp = *rh;
	if (nil == last)
	{
		ap->rgnsets = rp->next;
	}
	else
	{
		(**last).next = rp->next;
	}

	old_zone = GetZone();
	if (zone != old_zone)
	{
		// TODO: Do we ever see this?
		DebugStr("\pdispose_rgnset: zones did differ");
	}
	SetZone(zone);

	DisposeRgn_orig(rp->copy);
	DisposeRgn_orig(rp->big);
	DisposeHandle((Handle)rh);

	SetZone(old_zone);

end:
	;
}

static rgnset_h find_rgnset(RgnHandle original)
{
	rgnset_h rh;
	rgnset_p rp;

	rh = (**find_app_data(HandleZone((Handle)original))).rgnsets;

	while (nil != rh)
	{
		rp = *rh;
		if (original == rp->original) break;
		rh = rp->next;
	}

	return rh;
}

static rgnset_h new_rgnset(RgnHandle original)
{
	app_data_h ah;
	app_data_p ap;
	rgnset_h rh;
	rgnset_p rp;
	THz zone;
	THz old_zone;

//DebugStr("\pnew_rgnset");
	old_zone = GetZone();
	zone = HandleZone((Handle)original);
	if (zone != old_zone)
	{
		// TODO: Do we ever see this?
		DebugStr("\pnew_rgnset: zones did differ");
	}
	SetZone(zone);

	rh = (rgnset_h)NewHandle(sizeof **rh);
	require(rh, NewHandle);

	ah = find_app_data(zone);
	ap = *ah;

	rp = *rh;
	rp->original = original;
	rp->copy = NewRgn();
	rp->big = NewRgn();
	rp->next = ap->rgnsets;
	ap->rgnsets = rh;

NewHandle:
	SetZone(old_zone);

	return rh;
}

rgnset_h get_rgnset(RgnHandle original)
{
	rgnset_h rh;
	rgnset_p rp;
	RgnHandle copy, big;

	rh = find_rgnset(original);
	if (nil == rh)
	{
		rh = new_rgnset(original);
	}
	require(rh, end);
	rp = *rh;

	// If the original differs from the copy, recreate the copy. It will differ
	// right after it's created but also if an app loads a region from disk or
	// manipulates a region without going through a patched trap. For example,
	// BitMapToRgn was available as object code.
	copy = rp->copy;
	if (!EqualRgn(original, copy))
	{
		CopyRgn_orig(original, copy);
		big = rp->big;
		SetHandleSize((Handle)big, GetHandleSize((Handle)original));
		embiggen_rgn(original, big);
	}

end:
	return rh;
}

qd_globals_t *get_qd_globals(void)
{
	// CurrentA5 contains the value of the application's A5 register, which is
	// a pointer to its A5 world, which contains its globals. The global at
	// offset zero is GrafPtr thePort, the last of the QuickDraw globals.
	// Compute and return the address of the start of the QuickDraw globals.
	return (qd_globals_t *)(*(Ptr *)CurrentA5 + sizeof(GrafPtr) - sizeof(qd_globals_t));
}

void get_rgntmp(rgntmp_t *tmp)
{
	*tmp = get_qd_globals()->rgntmp;
}

void set_rgntmp(rgntmp_t *tmp)
{
	get_qd_globals()->rgntmp = *tmp;
}
