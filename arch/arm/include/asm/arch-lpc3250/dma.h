#ifndef LPC32XX_DMA_H
#define LPC32XX_DMA_H 1

#define LPC32XX_DMA_BASE 0x31000000
#define LPC32XX_DMA_CHANNELS 8

struct lpc32xx_dma_channel_regs {
	u32 src;
	u32 dest;
	u32 lli;
	u32 control;
	u32 config;
	u32 reserved[3];
};

#define DMA_CONFIG_ENABLE (1 << 0)
#define DMA_CONFIG_SRC(n) (((n) & 0x1f) << 1)
#define DMA_CONFIG_DEST(n) (((n) & 0x1f) << 6)
#define DMA_CONFIG_FLOW_M2P (1 << 11)
#define DMA_CONFIG_FLOW_P2M (2 << 11)

enum dma_peripheral {
	DMA_PERIPHERAL_I2S0_DMA0,
	DMA_PERIPHERAL_NAND1,
};

#define DMA_CTRL_SRC_BURST_1 (0)
#define DMA_CTRL_SRC_BURST_4 (1 << 15)
#define DMA_CTRL_SRC_WIDTH_32 (1 << 19)
#define DMA_CTRL_SRC_AUTOINC (1 << 26)

#define DMA_CTRL_DEST_BURST_1 (0)
#define DMA_CTRL_DEST_BURST_4 (1 << 12)
#define DMA_CTRL_DEST_WIDTH_32 (1 << 22)
#define DMA_CTRL_DEST_AHB1 (1 << 25)
#define DMA_CTRL_DEST_AUTOINC (1 << 27)

#define DMA_CTRL_INT_TC_EN (1 << 31)

struct lpc32xx_dma_regs {
	u32 int_stat;
	u32 int_tc_stat;
	u32 int_tc_clear;
	u32 int_err_stat;
	u32 int_err_clear;
	u32 raw_tc_stat;
	u32 raw_err_stat;
	u32 enable;
	u32 sw_burst_req;
	u32 sw_single_req;
	u32 sw_last_burst_req;
	u32 sw_last_single_req;
	u32 config;
	u32 sync;
	u32 reserved[50];
	struct lpc32xx_dma_channel_regs channel[LPC32XX_DMA_CHANNELS];
};

struct lpc32xx_dma_desc {
	u32 src;
	u32 dest;
	u32 lli;
	u32 control;
};

int lpc32xx_dma_get_channel(void);
int lpc32xx_dma_start_xfer(int channel, const struct lpc32xx_dma_desc *desc, u32 config);
int lpc32xx_dma_wait_status(int channel);
void lpc32xx_dma_put_channel(int channel);

#endif /* LPC32XX_DMA_H */
