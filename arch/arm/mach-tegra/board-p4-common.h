/*
 * arch/arm/mach-tegra/board-p4.h
 *
 * Copyright (C) 2016 ryang <decatf@gmail.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef _MACH_TEGRA_BOARD_P4_COMMON_H
#define _MACH_TEGRA_BOARD_P4_COMMON_H

extern void cmc623_resume(void);
extern void cmc623_suspend(void);


static int display1_resume(void) {
	cmc623_resume();
	return 0;
}

static int display1_suspend(void) {
	cmc623_suspend();
	return 0;
}

#endif /* _MACH_TEGRA_BOARD_P4_COMMON_H */
