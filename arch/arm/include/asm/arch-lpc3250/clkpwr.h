#ifndef LPC32XX_CLKPWR_H
#define LPC32XX_CLKPWR_H 1

#include <asm/types.h>

#define LPC32XX_CLKPWR	0x40004000

struct lpc32xx_clkpwr_regs {
	u32 reserved1[5];
	u32 bootmap;
	u32 p01_er;
	u32 usbclk_pdiv;
	u32 int_er;
	u32 int_rs;
	u32 int_sr;
	u32 int_ap;
	u32 pin_er;
	u32 pin_rs;
	u32 pin_sr;
	u32 pin_ap;
	u32 hclk_div;
	u32 pwr_ctrl;
	u32 pll397_ctrl;
	u32 main_osc_ctrl;
	u32 sysclk_ctrl;
	u32 lcdclk_ctrl;
	u32 hclk_pll_ctrl;
	u32 reserved2;
	u32 adc_clk_ctrl_1;
	u32 usb_ctrl;
	u32 sdram_clk_ctrl;
	u32 ddr_lap_nom;
	u32 ddr_lap_count;
	u32 ddr_cal_delay;
	u32 ddr_ssp_blk_ctrl;
	u32 i2s_clk_ctrl;
	u32 ms_ctrl;
	u32 reserved4[3];
	u32 macclk_ctrl;
	u32 reserved5[4];
	u32 test_clk_sel;
	u32 sw_int;
	u32 i2c_clk_ctrl;
	u32 key_clk_ctrl;
	u32 adc_clk_ctrl;
	u32 pwm_clk_ctrl;
	u32 timer_clk_ctrl;
	u32 timers_pwms_clk_ctrl;
	u32 spi_clk_ctrl;
	u32 nand_clk_ctrl;
	u32 reserved6;
	u32 uart3_clk_ctrl;
	u32 uart4_clk_ctrl;
	u32 uart5_clk_ctrl;
	u32 uart6_clk_ctrl;
	u32 irda_clk_ctrl;
	u32 uart_clk_ctrl;
	u32 dma_clk_ctrl;
	u32 auto_clk;
};

#define CLKPWR_MOSC_CTRL_DISABLE (1 << 0)
#define CLKPWR_MOSC_CTRL_CAP_MASK (0x7f << 2)
#define CLKPWR_MOSC_CTRL_CAP(n) (((n) & 0x7f) << 2)

#define CLKPWR_PLL397_CTRL_STS (1 << 0)
#define CLKPWR_PLL397_CTRL_DISABLE (1 << 1)
#define CLKPWR_PLL397_CTRL_BYPASS (1 << 9)
#define CLKPWR_PLL397_CTRL_BIAS_MASK 0x1c0

#define CLKPWR_PWR_CTRL_FORCE_PCLK (1 << 10)

enum clkpwr_mode {
	CLKPWR_MODE_RUN,
	CLKPWR_MODE_DIRECT,
	CLKPWR_MODE_STOP,
};

enum clkpwr_osc {
	CLKPWR_OSC_MAIN,
	CLKPWR_OSC_PLL397,
};

enum clkpwr_pll {
	CLKPWR_PLL_PLL397,
	CLKPWR_PLL_HCLK,
	CLKPWR_PLL_USB,
};

enum clkpwr_clk {
	CLKPWR_CLK_MAINOSC,
	CLKPWR_CLK_RTC,
	CLKPWR_CLK_SYS,
	CLKPWR_CLK_ARM,
	CLKPWR_CLK_HCLK,
	CLKPWR_CLK_PERIPH,
	CLKPWR_CLK_USB,
	CLKPWR_CLK_48M,
	CLKPWR_CLK_DDR,
	CLKPWR_CLK_MSSD,
};

#endif /* LPC32XX_CLKPWR_H */
