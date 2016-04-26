/*
 * Copyright 2016 Avionic Design GmbH
 * Copyright 2016 Julian Scheel <julian@jusst.de>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <watchdog.h>
#include <asm/io.h>
#include <asm/arch-tegra/wdt.h>
#include <asm/arch-tegra/tegra.h>

/* Timeout in seconds */
#define WDT_TIMEOUT 60

/* Timer to use - 5 is used in linux kernel */
#define WDT_TIMER_ID 5
void hw_watchdog_init(void)
{
	struct timer_ctrl *timer = (struct timer_ctrl *)NV_PA_TMR5_BASE;
	struct wdt_ctrl *wdt = (struct wdt_ctrl *)NV_PA_TMRWDT0_BASE;
	u32 val;

	/* Timer runs fixed at 1 MHz, reset is triggered at 4th timeout of
	 * timer */
	val = 1000000ul / 4;
	val |= (TIMER_PTV_EN | TIMER_PTV_PERIODIC);
	writel(val, &timer->ptv);

	/* Setup actual wdt */
	val = WDT_TIMER_ID |
		((WDT_TIMEOUT << WDT_CFG_PERIOD_SHIFT) & WDT_CFG_PERIOD_MASK) |
		WDT_CFG_PMC2CAR_RST_EN;
	writel(val, &wdt->config);

	/* Activate the wdt */
	writel(WDT_CMD_START_COUNTER, &wdt->command);
}

void hw_watchdog_reset(void)
{
	struct wdt_ctrl *wdt = (struct wdt_ctrl *)NV_PA_TMRWDT0_BASE;

	/* Activate the wdt */
	writel(WDT_CMD_START_COUNTER, &wdt->command);
}

void hw_watchdog_disable(void)
{
	struct timer_ctrl *timer = (struct timer_ctrl *)NV_PA_TMR5_BASE;
	struct wdt_ctrl *wdt = (struct wdt_ctrl *)NV_PA_TMRWDT0_BASE;

	/* Write unlock pattern */
	writel(WDT_UNLOCK_PATTERN, &wdt->unlock);
	/* Disable wdt */
	writel(WDT_CMD_DISABLE_COUNTER, &wdt->command);
	/* Stop timer */
	writel(0, &timer->ptv);
}
