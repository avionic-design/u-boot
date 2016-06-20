/*
 * Copyright (c) 2016 Julian Scheel <julian@jusst.de>
 * Copyright (c) 2016 Avionic Design GmbH
 *
 * SPDX-License-Identifier:	GPL-2.0
 */

#ifndef _APBDMA_H_
#define _APBDMA_H_

struct apbdma_ctlr {
	u32 command;
	u32 status;
	u32 reserved[2];
	u32 counter;
	u32 irq_sta_cpu;
	u32 irq_sta_cop;
	u32 irq_mask;
	u32 irq_mask_set;
	u32 irq_mask_clr;
	u32 trig_reg;
	u32 channel_trig_reg;
	u32 dma_status;
	u32 channel_en_reg;
	u32 security_reg;
	u32 channel_swid;
	u32 reserved1;
	u32 chan_wt_reg0;
	u32 chan_wt_reg1;
	u32 chan_wt_reg2;
	u32 chan_wt_reg3;
	u32 channel_swid1;
};

#define APBDMA_GEN_SHIFT 31

struct apbdma_channel {
	u32 csr;
	u32 sta;
	u32 byte_sta;
	u32 csre;
	u32 ahb_ptr;
	u32 ahb_seq;
	u32 apb_ptr;
	u32 apb_seq;
	u32 wcount;
	u32 word_transfer;
};

#define CSR_ENABLE_SHIFT 31
#define CSR_HOLD_SHIFT 29
#define CSR_DIR_SHIFT 28
#define CSR_ONCE_SHIFT 27
#define CSR_FLOW_SHIFT 21
#define CSR_REQ_SEL_MASK 0x1f
#define CSR_REQ_SEL_SHIFT 16

#define AHB_BUS_WIDTH_MASK 0x7
#define AHB_BUS_WIDTH_SHIFT 28

#define BUS_WIDTH_8 0
#define BUS_WIDTH_16 1
#define BUS_WIDTH_32 2
#define BUS_WIDTH_64 3
#define BUS_WIDTH_128 4

#define AHB_BURST_MASK 0x7
#define AHB_BURST_SHIFT 24

#define BURST_1 4
#define BURST_4 5
#define BURST_8 6

#endif
