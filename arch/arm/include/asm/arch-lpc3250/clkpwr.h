#ifndef LPC32XX_CLKPWR_H
#define LPC32XX_CLKPWR_H 1

#include <asm/types.h>

#define LPC32XX_CLKPWR_BASE 0x40004000

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
	u32 mac_ctrl;
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

#define MAIN_OSC_FREQ 13000000
#define CLOCK_OSC_FREQ 32768

#define CLKPWR_SDRCLK_USE_DDR (1 << 1)
#define CLKPWR_SDRCLK_DQS_DLY(n) (((n) & 0x1f) << 2)
#define CLKPWR_SDRCLK_CAL_ON_RTC (1 << 7)
#define CLKPWR_SDRCLK_DO_CAL (1 << 8)
#define CLKPWR_SDRCLK_USE_CAL (1 << 9)
#define CLKPWR_SDRCLK_SENS_FACT(n) (((n) & 0x7) << 10)
#define CLKPWR_SDRCLK_HCLK_DLY(n) (((n) & 0x1f) << 14)
#define CLKPWR_SDRCLK_SW_DDR_RESET (1 << 19)

#define CLKPWR_STOP_MODE_CTRL (1 << 0)
#define CLKPWR_SELECT_RUN_MODE (1 << 2)

#define CLKPWR_SYSCTRL_SYSCLKMUX (1 << 0)
#define CLKPWR_SYSCTRL_USEPLL397 (1 << 1)
#define CLKPWR_SYSCTRL_BP_TRIG(n) (((n) & 0x3ff) << 2)
#define CLKPWR_SYSCTRL_BP_MASK (0x3ff << 2)

#define CLKPWR_HCLKPLL_PLL_STS (1 << 0)
#define CLKPWR_HCLKPLL_PLLM(n) (((n) & 0xff) << 1)
#define CLKPWR_HCLKPLL_PREDIV_PLUS1(n) (((n) & 0x3) << 9)
#define CLKPWR_HCLKPLL_POSTDIV_2POW(n) (((n) & 0x3) << 11)
#define CLKPWR_HCLKPLL_FDBK_SEL_FCLK (1 << 13)
#define CLKPWR_HCLKPLL_POSTDIV_BYPASS (1 << 14)
#define CLKPWR_HCLKPLL_CCO_BYPASS (1 << 15)
#define CLKPWR_HCLKPLL_POWER_UP (1 << 16)

#define CLKPWR_USBCTRL_PLL_STS (1 << 0)
#define CLKPWR_USBCTRL_PLL_PWRUP (1 << 16)

#define CLKPWR_HCLKDIV_DIV_2POW(n) ((n) & 0x3)
#define CLKPWR_HCLKDIV_PCLK_DIV(n) (((n) & 0x1f) << 2)
#define CLKPWR_HCLKDIV_DDRCLK_STOP (0x0 << 7)
#define CLKPWR_HCLKDIV_DDRCLK_NORM (0x1 << 7)
#define CLKPWR_HCLKDIV_DDRCLK_HALF (0x2 << 7)

#define CLKPWR_MOSC_CTRL_DISABLE (1 << 0)
#define CLKPWR_MOSC_CTRL_CAP_MASK (0x7f << 2)
#define CLKPWR_MOSC_CTRL_CAP(n) (((n) & 0x7f) << 2)

#define CLKPWR_PLL397_CTRL_STS (1 << 0)
#define CLKPWR_PLL397_CTRL_DISABLE (1 << 1)
#define CLKPWR_PLL397_CTRL_BYPASS (1 << 9)
#define CLKPWR_PLL397_CTRL_BIAS_MASK 0x1c0

#define CLKPWR_PWR_CTRL_FORCE_PCLK (1 << 10)

#define CLKPWR_NAND_CLK_CTRL_SLC_ENABLE (1 << 0)
#define CLKPWR_NAND_CLK_CTRL_MLC_ENABLE (1 << 1)
#define CLKPWR_NAND_CLK_CTRL_SLC_SELECT (1 << 2)

#define CLKPWR_MAC_CTRL_HRCCLK_EN (1 << 0)
#define CLKPWR_MAC_CTRL_MMIOCLK_EN (1 << 1)
#define CLKPWR_MAC_CTRL_DMACLK_EN (1 << 2)
#define CLKPWR_MAC_CTRL_PINS_NONE 0x00
#define CLKPWR_MAC_CTRL_PINS_MII 0x08
#define CLKPWR_MAC_CTRL_PINS_RMII 0x18

#define CLKPWR_TEST_CLK_OUTPUT_EN (1<<4)

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

unsigned int sys_get_rate(enum clkpwr_clk clock);

#endif /* LPC32XX_CLKPWR_H */
