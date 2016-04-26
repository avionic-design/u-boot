/*
 * Copyright 2016 Avionic Design GmbH
 * Copyright 2016 Julian Scheel <julian@jusst.de>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _TEGRA_WDT_H_
#define _TEGRA_WDT_H_

struct wdt_ctrl {
	u32 config;
	u32 status;
	u32 command;
	u32 unlock;
};

#define WDT_CFG_SOURCE_MASK 0xf

#define WDT_CFG_PERIOD_SHIFT 4
#define WDT_CFG_PERIOD_MASK (0xff << WDT_CFG_PERIOD_SHIFT)

#define WDT_CFG_PMC2CAR_RST_EN (1 << 15)

#define WDT_STS_COUNT_SHIFT 4
#define WDT_STS_COUNT_MASK (0xff << WDT_STS_COUNT_SHIFT)

#define WDT_CMD_START_COUNTER (1 << 0)
#define WDT_CMD_DISABLE_COUNTER (1 << 1)

#define WDT_UNLOCK_PATTERN 0xc45a

/* Timer registers */
struct timer_ctrl {
	u32 ptv;
};

#define TIMER_PTV_EN (1 << 31)
#define TIMER_PTV_PERIODIC (1 << 30)

#endif /* _TEGRA_WDT_H_ */
