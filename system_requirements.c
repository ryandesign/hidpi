/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

#include "system_requirements.h"

#include "macros.h"

Boolean system_requirements_met(void)
{
	SysEnvRec env;
	Boolean result = false;

	// Ensure the ROM is new enough.
	require(has_128k_rom(), has_128k_rom);

	// Ensure the OS is new enough.
	SysEnvirons(curSysEnvVers, &env);
	require(env.systemVersion >= 0x0410, systemVersion);

	// Not compatible with Color QuickDraw yet.
	nrequire(env.hasColorQD, hasColorQD);

	// Done!
	result = true;
	goto done;

hasColorQD:
systemVersion:
has_128k_rom:

done:
	return result;
}
