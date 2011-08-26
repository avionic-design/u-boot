#ifndef LPC32XX_DMA_H
#define LPC32XX_DMA_H 1

#define LPC32XX_DMA 0x31000000
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

#endif /* LPC32XX_DMA_H */
