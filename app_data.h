/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#ifndef HIDPI_APP_DATA_H
#define HIDPI_APP_DATA_H

#include "globals.h"

void dispose_app_data(THz zone);
app_data_h find_app_data(THz zone);
app_data_h new_app_data(THz zone);

#endif
