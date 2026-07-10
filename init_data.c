/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#include "init_data.h"

#include "macros.h"

Boolean init_data(data_t *data)
{
	CrsrBusy = true; // TODO: check if necessary

	{
		Point hotspot = TheCrsr.hotSpot;

		//pin_hotspot(&hotspot);
		*(long *)&data->hotspot_2x = *(long *)&hotspot * k_scale;
	}

	CrsrBusy = false; // TODO: check if necessary

	return true;
}
