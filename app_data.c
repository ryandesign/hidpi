/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#include "app_data.h"

#include "macros.h"

void dispose_app_data(THz zone)
{
	app_data_h last = nil;
	app_data_h ah;
	app_data_p ap;

	ah = g_data->app_data;
	while (nil != ah)
	{
		ap = *ah;
		if (zone == ap->zone) break;
		last = ah;
		ah = ap->next;
	}
	require(ah, end);

	ap = *ah;
	if (nil == last)
	{
		g_data->app_data = ap->next;
	}
	else
	{
		(**last).next = ap->next;
	}

end:
	;
}

app_data_h find_app_data(THz zone)
{
	app_data_h ah;
	app_data_p ap;

	ah = g_data->app_data;
	while (nil != ah)
	{
		ap = *ah;
		if (zone == ap->zone) break;
		ah = ap->next;
	}

	if (nil == ah)
	{
		ah = &g_data->system_data_p;
	}

	return ah;
}

app_data_h new_app_data(THz zone)
{
	register app_data_h ah;
	register app_data_p ap;

#if __option(a4_globals)
	ah = (app_data_h)NewHandleSys(sizeof **ah);
#else
	ah = (app_data_h)NewHandle(sizeof **ah);
#endif
	require(ah, NewHandle);

	ap = *ah;
	ap->zone = zone;
	ap->rgnsets = nil;
	ap->next = g_data->app_data;
	g_data->app_data = ah;

NewHandle:
	return ah;
}
