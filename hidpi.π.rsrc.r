/*
SPDX-FileCopyrightText: © 2026 Ryan Carsten Schmidt <https://github.com/ryandesign>
SPDX-License-Identifier: MIT
*/

resource 'MBAR' (128, purgeable) {
	{	/* array MenuArray: 4 elements */
		/* [1] */
		128,
		/* [2] */
		129,
		/* [3] */
		130,
		/* [4] */
		131
	}
};

resource 'MENU' (128) {
	128,
	textMenuProc,
	allEnabled,
	enabled,
	apple,
	{	/* array: 0 elements */
	}
};

resource 'MENU' (129) {
	129,
	textMenuProc,
	0x7FFFFFF7,
	enabled,
	"File",
	{	/* array: 5 elements */
		/* [1] */
		"New Window", noIcon, "N", noMark, plain,
		/* [2] */
		"New Window @2x", noIcon, "2", noMark, plain,
		/* [3] */
		"Close Window", noIcon, "W", noMark, plain,
		/* [4] */
		"-", noIcon, noKey, noMark, plain,
		/* [5] */
		"Quit", noIcon, "Q", noMark, plain
	}
};

resource 'MENU' (130) {
	130,
	textMenuProc,
	0x7FFFFFFD,
	enabled,
	"Edit",
	{	/* array: 6 elements */
		/* [1] */
		"Undo", noIcon, "Z", noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"Cut", noIcon, "X", noMark, plain,
		/* [4] */
		"Copy", noIcon, "C", noMark, plain,
		/* [5] */
		"Paste", noIcon, "V", noMark, plain,
		/* [6] */
		"Clear", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (131) {
	131,
	textMenuProc,
	allEnabled,
	enabled,
	"Debug",
	{	/* array: 1 elements */
		/* [1] */
		"System Error", noIcon, noKey, noMark, plain
	}
};

data 'RMAP' (128, "mstr", purgeable) {
	$"5354 5220 0000 0000"
};

resource 'STR#' (128) {
	{	/* array StringArray: 2 elements */
		/* [1] */
		"Hello, ",
		/* [2] */
		"world."
	}
};

resource 'WIND' (128) {
	{48, 8, 191, 191},
	documentProc,
	invisible,
	goAway,
	0x0,
	"New Window"
};

resource 'mstr' (100, purgeable) {
	"File"
};

resource 'mstr' (101, purgeable) {
	"Quit"
};

