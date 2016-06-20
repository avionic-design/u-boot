#ifndef _TEGRA_DMA_H_
#define _TEGRA_DMA_H_

enum apbdma_req_sel {
	CNTR_REQ = 0,
	APBIF_CH0,
	APBIF_CH1,
	APBIF_CH2,
	APBIF_CH3,
	HSI,
	APBIF_CH4,
	APBIF_CH5,
	UART_A,
	UART_B,
	UART_C,
	DTV,
	APBIF_CH6,
	APBIF_CH7,
	APBIF_CH8,
	SL2B1,
	SL2B2,
	SL2B3,
	SL2B4,
	UART_D,
	I2C = 21,
	I2C2,
	I2C3,
	DVC_I2C,
	OWR,
	I2C4,
	SL2B5,
	SL2B6,
	APBIF_CH9,
	I2C6,
	NA32
};

enum apbdma_direction {
	APB_TO_AHB,
	AHB_TO_APB
};

/*
 * Initiate a DMA transfer between AHB and APB
 * The flow is synchronized by the specified req_sel trigger
 */
int tegra_dma_transfer(u32 ahb_addr, u32 apb_addr, enum apbdma_direction dir,
		enum apbdma_req_sel req, unsigned length, bool sync);
u32 tegra_dma_get_status();
u32 tegra_dma_get_byte_status();
int tegra_dma_set_enable(bool enable);

#endif
