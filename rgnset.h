/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#ifndef HIDPI_RGNSET_PATCH
#define HIDPI_RGNSET_PATCH

#include "globals.h"

void dispose_rgnset(RgnHandle original);
rgnset_h get_rgnset(RgnHandle original);
qd_globals_t *get_qd_globals(void);
void get_rgntmp(rgntmp_t *tmp);
void set_rgntmp(rgntmp_t *tmp);

#endif
