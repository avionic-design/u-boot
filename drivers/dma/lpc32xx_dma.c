/*
 * Copyright (C) 2008 by NXP Semiconductors
 * All rights reserved.
 * 
 * @Author: Kevin Wells
 * @Descr: LPC3250 DMA controller interface support functions
 *  
 * See file CREDITS for list of people who contributed to this
 * project.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *  
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *  
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/io.h>
#include <asm/system.h>
#include <asm/arch/clkpwr.h>
#include <asm/arch/dma.h>

#ifdef CONFIG_SPL_BUILD
#  define printf(...) do { } while (0)
#endif

#define BIT(x) (1 << (x))
#define DMA_CLK_ENABLE 1

/**********************************************************************
* DMA controller register structures
**********************************************************************/

/*
 * WARNING: This breaks for SPL builds! The reason is that SPL builds
 *          don't clear the BSS section, so the value of the "allocated"
 *          variable is undefined.
 */
static u32 allocated = 0;

int lpc32xx_dma_get_channel(void)
{
	struct lpc32xx_clkpwr_regs *clkpwr =
		(struct lpc32xx_clkpwr_regs *)LPC32XX_CLKPWR_BASE;
	struct lpc32xx_dma_regs *dma =
		(struct lpc32xx_dma_regs *)LPC32XX_DMA_BASE;
	u32 value;
	int i;

#ifdef CONFIG_SPL_BUILD
	allocated = 0;
#endif

	if (!allocated) { /* First time caller */
		value = readl(&clkpwr->dma_clk_ctrl);
		value |= DMA_CLK_ENABLE;
		writel(value, &clkpwr->dma_clk_ctrl);

		/*
		 * Make sure DMA controller and all channels are disabled.
		 * Controller is in little-endian mode. Disable sync signals.
		 */
		writel(0, &dma->config);
		writel(0, &dma->sync);

		/* Clear interrupt and error statuses */
		writel(0xff, &dma->int_tc_clear);
		//writel(0xff, &dma->raw_tc_stat);
		writel(0xff, &dma->int_err_clear);
		//writel(0xff, &dma->raw_err_stat);

		/* Enable DMA controller */
		writel(DMA_CONFIG_ENABLE, &dma->config);
	}

	for (i = 0; i < LPC32XX_DMA_CHANNELS; i++) {
		if ((allocated & BIT(i)) == 0)
			break;
	}

	/* Check if all the available channles are busy */
	if (i == LPC32XX_DMA_CHANNELS)
		return -1;

	allocated |= BIT(i);
	return i;
}

int lpc32xx_dma_start_xfer(int channel, const struct lpc32xx_dma_desc *desc,
		u32 config)
{
	struct lpc32xx_dma_regs *dma =
		(struct lpc32xx_dma_regs *)LPC32XX_DMA_BASE;

	if ((allocated & BIT(channel)) == 0) {
		printf ("ERR: Request for xfer on "
		       "unallocated channel %d\r\n", channel);
		BUG();
	}

	writel(BIT(channel), &dma->int_tc_clear);
	writel(BIT(channel), &dma->int_err_clear);

	writel(desc->src, &dma->channel[channel].src);
	writel(desc->dest, &dma->channel[channel].dest);
	writel(desc->lli, &dma->channel[channel].lli);
	writel(desc->control, &dma->channel[channel].control);
	writel(config, &dma->channel[channel].config);

	return 0;
}

int lpc32xx_dma_wait_status(int channel)
{
	struct lpc32xx_dma_regs *dma =
		(struct lpc32xx_dma_regs *)LPC32XX_DMA_BASE;

	while (1) {
		if ((readl(&dma->raw_err_stat) & BIT(channel)) ||
		    (readl(&dma->raw_tc_stat) & BIT(channel)))
			break;
	}

	if (readl(&dma->raw_err_stat) & BIT(channel)) {
		writel(BIT(channel), &dma->int_err_clear);
		return -1;
	}

	writel(BIT(channel), &dma->int_tc_clear);
	return 0;
}

void lpc32xx_dma_put_channel(int channel)
{
	struct lpc32xx_clkpwr_regs *clkpwr =
		(struct lpc32xx_clkpwr_regs *)LPC32XX_CLKPWR_BASE;
	struct lpc32xx_dma_regs *dma =
		(struct lpc32xx_dma_regs *)LPC32XX_DMA_BASE;
	u32 value;

	/* Check if given channel no is valid */
	if ((channel < 0) || (channel >= LPC32XX_DMA_CHANNELS))
		return;

	allocated &= ~BIT(channel);

	/* Shut down channel */
	writel(0, &dma->channel[channel].control);
	writel(0, &dma->channel[channel].config);

	value = readl(&dma->sync);
	value &= ~BIT(channel);
	writel(value, &dma->sync);

	if (!allocated) {
		/* Disable DMA controller */
		value = readl(&dma->config);
		value &= ~DMA_CONFIG_ENABLE;
		writel(value, &dma->config);

		/* If all channels are free disable the clock */
		value = readl(&clkpwr->dma_clk_ctrl);
		value &= ~DMA_CLK_ENABLE;
		writel(value, &clkpwr->dma_clk_ctrl);
	}
}
