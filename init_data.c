/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#include "init_data.h"

#include "macros.h"
#include "missing_traps.h"

Boolean init_data(data_t *data)
{
	CrsrBusy = true; // TODO: check if necessary
	//enlarge_cursor();
	{
		Point hotspot = TheCrsr.hotSpot;

		//pin_hotspot(&hotspot);
		*(long *)&data->hotspot_2x = *(long *)&hotspot * k_scale;
	}
	// TODO: Color QuickDraw
	BlockMoveData(CrsrSave, data->save_2x, sizeof(long) * rect_height(&CrsrRect));
	CrsrBusy = false; // TODO: check if necessary

	return true;
}
