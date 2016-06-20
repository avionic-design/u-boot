/*
 * Copyright (c) 2016 Julian Scheel <julian@jusst.de>
 * Copyright (c) 2016 Avionic Design GmbH
 *
 * SPDX-License-Identifier:	GPL-2.0
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/apbdma.h>
#include "tegra-dma.h"

int tegra_dma_set_bus_width(int ahb_width, int ahb_burst, int apb_width)
{
	/* channel 0 */
	struct apbdma_channel *channel = (struct apbdma_channel*) (NV_PA_APBDMA_BASE + 0x1000);

	u32 ahb_seq = (1 << 31);
	u32 apb_seq = (1 << 16);
	ahb_seq &= ~((AHB_BUS_WIDTH_MASK << AHB_BUS_WIDTH_SHIFT) |
			(AHB_BURST_MASK << AHB_BURST_SHIFT));
	apb_seq &= ~(AHB_BUS_WIDTH_MASK << AHB_BUS_WIDTH_SHIFT);
	switch (ahb_width) {
		case 8:
			ahb_seq |= BUS_WIDTH_8 << AHB_BUS_WIDTH_SHIFT;
			break;
		case 16:
			ahb_seq |= BUS_WIDTH_16 << AHB_BUS_WIDTH_SHIFT;
			break;
		case 32:
			ahb_seq |= BUS_WIDTH_32 << AHB_BUS_WIDTH_SHIFT;
			break;
		case 64:
			ahb_seq |= BUS_WIDTH_64 << AHB_BUS_WIDTH_SHIFT;
			break;
		case 128:
			ahb_seq |= BUS_WIDTH_128 << AHB_BUS_WIDTH_SHIFT;
			break;
		default:
			error("%s: Unsupported bus width specified\n", __func__);
			return -1;
	}
	switch (ahb_burst) {
		case 1:
			ahb_seq |= BURST_1 << AHB_BURST_SHIFT;
			break;
		case 4:
			ahb_seq |= BURST_4 << AHB_BURST_SHIFT;
			break;
		case 8:
			ahb_seq |= BURST_8 << AHB_BURST_SHIFT;
			break;
		default:
			error("%s: Unsupported burst specified\n", __func__);
			return -1;
	}

	switch (apb_width) {
		case 8:
			apb_seq |= BUS_WIDTH_8 << AHB_BUS_WIDTH_SHIFT;
			break;
		case 16:
			apb_seq |= BUS_WIDTH_16 << AHB_BUS_WIDTH_SHIFT;
			break;
		case 32:
			apb_seq |= BUS_WIDTH_32 << AHB_BUS_WIDTH_SHIFT;
			break;
		case 64:
			apb_seq |= BUS_WIDTH_64 << AHB_BUS_WIDTH_SHIFT;
			break;
		case 128:
			apb_seq |= BUS_WIDTH_128 << AHB_BUS_WIDTH_SHIFT;
			break;
		default:
			error("%s: Unsupported bus width specified\n", __func__);
			return -1;
	}

	debug("%s: ahb_seq: %p=0x%x\n", __func__, &channel->ahb_seq, ahb_seq);
	debug("%s: apb_seq: %p=0x%x\n", __func__, &channel->apb_seq, apb_seq);
	writel(ahb_seq, &channel->ahb_seq);
	writel(apb_seq, &channel->apb_seq);

	return 0;
}

int tegra_dma_set_enable(bool enable)
{
	struct apbdma_channel *channel = (struct apbdma_channel*) (NV_PA_APBDMA_BASE + 0x1000);

	u32 csr = readl(&channel->csr);
	csr &= ~(1 << CSR_ENABLE_SHIFT);
	csr |= !!enable << CSR_ENABLE_SHIFT;
	debug("%s: csr: 0x%x\n", __func__, csr);
	writel(csr, &channel->csr);

	debug("%s: ahb ptr: %p=0x%x, apb_ptr: %p=0x%x\n", __func__,
			&channel->ahb_ptr, readl(&channel->ahb_ptr),
			&channel->apb_ptr, readl(&channel->apb_ptr));
	return 0;
}

int tegra_dma_set_csr(bool hold, enum apbdma_direction dir, bool flow, enum apbdma_req_sel req)
{
	struct apbdma_channel *channel = (struct apbdma_channel*) (NV_PA_APBDMA_BASE + 0x1000);

	u32 csr = readl(&channel->csr);
	csr &= ~((1 << CSR_HOLD_SHIFT) | (1 << CSR_DIR_SHIFT) |
			(1 << CSR_FLOW_SHIFT) | (1 << CSR_ONCE_SHIFT) |
			(CSR_REQ_SEL_MASK << CSR_REQ_SEL_SHIFT));
	csr |= (!!hold << CSR_HOLD_SHIFT) | (dir << CSR_DIR_SHIFT) |
		(req << CSR_REQ_SEL_SHIFT) | (!!flow << CSR_FLOW_SHIFT) | (1 << CSR_ONCE_SHIFT);
	writel(csr, &channel->csr);

	debug("%s: csr (%p, %p): 0x%x\n", __func__, channel, &channel->csr, csr);
	return 0;
}

int tegra_dma_transfer(u32 ahb_addr, u32 apb_addr, enum apbdma_direction dir,
		enum apbdma_req_sel req, unsigned length, bool sync)
{
	struct apbdma_ctlr *ctlr = (struct apbdma_ctlr*) NV_PA_APBDMA_BASE;
	/* channel 0 */
	struct apbdma_channel *channel = (struct apbdma_channel*) (NV_PA_APBDMA_BASE + 0x1000);

	writel(ahb_addr, &channel->ahb_ptr);
	writel(apb_addr, &channel->apb_ptr);
	writel(length * 4, &channel->wcount);

	if (tegra_dma_set_bus_width(32, 4, 32) < 0)
		return -1;

	if (tegra_dma_set_csr(sync, dir, true, req) < 0)
		return -1;

	/* global enable */
	writel(1 << APBDMA_GEN_SHIFT, &ctlr->command);

	/* channel enable */
	if (tegra_dma_set_enable(true) < 0)
		return -1;

	return 0;
}

u32 tegra_dma_get_status()
{
	struct apbdma_channel *channel = (struct apbdma_channel*) (NV_PA_APBDMA_BASE + 0x1000);

	return readl(&channel->sta);
}

u32 tegra_dma_get_byte_status()
{
	struct apbdma_channel *channel = (struct apbdma_channel*) (NV_PA_APBDMA_BASE + 0x1000);

	return readl(&channel->byte_sta);
}
