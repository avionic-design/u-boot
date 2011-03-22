/*
 * [origin: Linux kernel drivers/watchdog/davinci_wdt.c]
 *
 * Watchdog driver for DaVinci DM644x/DM646x processors
 *
 * Copyright (C) 2011 Avionic Design GmbH <alban.bedel@avionic-design.de>
 * Copyright (C) 2006 Texas Instruments.
 *
 * 2007 (c) MontaVista Software, Inc. This file is licensed under
 * the terms of the GNU General Public License version 2. This program
 * is licensed "as is" without any warranty of any kind, whether express
 * or implied.
 */

/*
 * The Watchdog Timer Mode Register can be only written to once. If the
 * timeout need to be set from U-Boot, be sure that the bootstrap doesn't
 * write to this register. Inform Linux to it too
 */

#include <common.h>
#include <watchdog.h>
#include <asm/arch/hardware.h>
#include <asm/io.h>

#define MAX_HEARTBEAT     600   /* really the max margin is 264/27MHz*/

/* Timer register set definition */
#define PID12   (0x0)
#define EMUMGT  (0x4)
#define TIM12   (0x10)
#define TIM34   (0x14)
#define PRD12   (0x18)
#define PRD34   (0x1C)
#define TCR     (0x20)
#define TGCR    (0x24)
#define WDTCR   (0x28)

/* TCR bit definitions */
#define ENAMODE12_DISABLED      (0 << 6)
#define ENAMODE12_ONESHOT       (1 << 6)
#define ENAMODE12_PERIODIC      (2 << 6)

/* TGCR bit definitions */
#define TIM12RS_UNRESET         (1 << 0)
#define TIM34RS_UNRESET         (1 << 1)
#define TIMMODE_64BIT_WDOG      (2 << 2)

/* WDTCR bit definitions */
#define WDEN                    (1 << 14)
#define WDFLAG                  (1 << 15)
#define WDKEY_SEQ0              (0xa5c6 << 16)
#define WDKEY_SEQ1              (0xda7e << 16)

static int heartbeat = CONFIG_WATCHDOG_TIMEOUT;
static u32 wdt_base  = CONFIG_SYS_WATCHDOG_BASE;

void hw_watchdog_reset(void)
{
	/* put watchdog in service state */
        writel(WDKEY_SEQ0, wdt_base + WDTCR);
        /* put watchdog in active state */
        writel(WDKEY_SEQ1, wdt_base + WDTCR);
}

void hw_watchdog_init(void)
{
	u32 tgcr;
	u32 timer_margin;
	unsigned long wdt_freq = CONFIG_SYS_WATCHDOG_FREQ;

	if (heartbeat > MAX_HEARTBEAT)
		heartbeat = MAX_HEARTBEAT;

	/* disable, internal clock source */
        writel(0, wdt_base + TCR);
        /* reset timer, set mode to 64-bit watchdog, and unreset */
        writel(0, wdt_base + TGCR);
        tgcr = TIMMODE_64BIT_WDOG | TIM12RS_UNRESET | TIM34RS_UNRESET;
        writel(tgcr, wdt_base + TGCR);
        /* clear counter regs */
        writel(0, wdt_base + TIM12);
        writel(0, wdt_base + TIM34);
        /* set timeout period */
        timer_margin = (((u64)heartbeat * wdt_freq) & 0xffffffff);
        writel(timer_margin, wdt_base + PRD12);
        timer_margin = (((u64)heartbeat * wdt_freq) >> 32);
        writel(timer_margin, wdt_base + PRD34);
        /* enable run continuously */
        writel(ENAMODE12_PERIODIC, wdt_base + TCR);
        /* Once the WDT is in pre-active state write to
         * TIM12, TIM34, PRD12, PRD34, TCR, TGCR, WDTCR are
         * write protected (except for the WDKEY field)
         */
        /* put watchdog in pre-active state */
        writel(WDKEY_SEQ0 | WDEN, wdt_base + WDTCR);
        /* put watchdog in active state */
        writel(WDKEY_SEQ1 | WDEN, wdt_base + WDTCR);
}
