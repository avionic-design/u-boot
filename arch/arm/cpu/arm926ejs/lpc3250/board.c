#include <common.h>

#include <asm/arch/clkpwr.h>
#include <asm/arch/emc.h>
#include <asm/arch/gpio.h>
#include <asm/io.h>

#include <div64.h>
#include <nand.h>

DECLARE_GLOBAL_DATA_PTR;

#if defined(CONFIG_BOOTSTRAP_BUILD) || defined(CONFIG_NAND_SPL)
static gd_t gdata __attribute__((section(".data")));
static bd_t bdata __attribute__((section(".data")));
#endif

typedef enum { false, true } bool;

#if CONFIG_PTIP_MACH_TYPE == MACH_TYPE_PTIP_CLASSIC
#define PTIP_CLASSIC_SETUP_RT_BL
#endif

#if defined(PTIP_CLASSIC_SETUP_RT_BL)
static void setup_rt_bl(void)
{
	struct lpc32xx_gpio_regs *gpio =
		(struct lpc32xx_gpio_regs *)LPC32XX_GPIO_BASE;
	struct lpc32xx_clkpwr_regs *clkpwr =
		(struct lpc32xx_clkpwr_regs *)LPC32XX_CLKPWR_BASE;
	u32 value;

	/*********** Boot-up settings for RT/BL ***********/
	/* Disable test clock output to GPO_00 */
	value = readl(&clkpwr->test_clk_sel);
	value &= ~CLKPWR_TEST_CLK_OUTPUT_EN;
	writel(value, &clkpwr->test_clk_sel);
	/* Set GPIO_00 (BL_active) as output */
	writel(P2_DIR_GPIO(0), &gpio->p2_dir_set);
	/* Set both outputs to high level */
	writel(P3_GPO(0) | P2_DIR_GPIO(0), &gpio->p3_out_set);

}
#endif

#if defined(CONFIG_BOOTSTRAP_BUILD) || defined(CONFIG_NAND_SPL)
static void setup_gpio(void)
{
	struct lpc32xx_gpio_regs *gpio =
		(struct lpc32xx_gpio_regs *)LPC32XX_GPIO_BASE;
	u32 value;

	value = P2_SDRAMD19D31_GPIO | P2_GPO21_U4TX | P2_GPIO03_KEYROW7 |
		P2_GPIO02_KEYROW6 | P2_GPIO05_SSEL0;
	writel(value, &gpio->p2_mux_clr);

	value = P2_DIR_GPIO(5);
	writel(value, &gpio->p2_dir_set);

	value = P_I2STXSDA1_MAT31 | P_I2STXCLK1_MAT30 | P_I2STXWS1_CAP30 |
		P_MAT20 | P_MAT21 | P_MAT03 | P_MAT02 | P_MAT01 | P_MAT00 |
		P_SPI1DATAIO_SSP0_MOSI | P_SPI1DATAIN_SSP0_MISO |
		P_SPI1CLK_SCK0;
	writel(value, &gpio->p_mux_set);

	value = P_SPI2DATAIO_MOSI1 | P_SPI2DATAIN_MISO1 | P_SPI2CLK_SCK1 |
		P_U7TX_MAT11;
	writel(value, &gpio->p_mux_clr);

	value = P3_GPO2_MAT10 | P3_GPO6 | P3_GPO8 | P3_GPO9 | P3_GPO10_MC2B |
		P3_GPO12_MC2A | P3_GPO13_MC1B | P3_GPO15_MC1A |
		P3_GPO16_MC0B | P3_GPO18_MC0A;
	writel(value, &gpio->p3_mux_clr);

	value = P0_GPO0_I2SRXCLK1 | P0_GPO1_I2SRXWS1 | P0_GPO2_I2SRXSDA0 |
		P0_GPO3_I2SRXCLK0 | P0_GPO4_I2SRXWS0 | P0_GPO5_I2STXSDA0 |
		P0_GPO6_I2STXCLK0 | P0_GPO7_I2STXWS0;
	writel(value, &gpio->p0_mux_clr);

	value = P0_GPO0_I2SRXCLK1 | P0_GPO1_I2SRXWS1;
	writel(value, &gpio->p0_dir_set);

	value = P0_GPO0_I2SRXCLK1 | P0_GPO1_I2SRXWS1;
	writel(value, &gpio->p0_out_clr);

	writel(P1_ALL, &gpio->p1_mux_clr);

	/*
	 * GPO( 4): ethernet PHY reset?
	 * GPO(19): NAND flash write-protection
	 */
	value = P3_GPO(4) | P3_GPO(19);
	writel(value, &gpio->p3_out_set);
}

struct clkpwr_pll_setup {
	u32 analog;
	u32 bypass;
	u32 direct;
	u32 fdbk;
	u32 pllp;
	u32 plln;
	u32 pllm;
};

static void clkpwr_set_hclk_divs(u32 dram, u32 periph, u32 hclk)
{
	struct lpc32xx_clkpwr_regs *clkpwr =
		(struct lpc32xx_clkpwr_regs *)LPC32XX_CLKPWR_BASE;
	u32 value = 0;

	switch (hclk) {
	case 1:
		value = 0;
		break;

	case 2:
		value = 1;
		break;

	case 4:
	default:
		value = 2;
		break;
	}

	value = dram | CLKPWR_HCLKDIV_PCLK_DIV(periph - 1) |
		CLKPWR_HCLKDIV_DIV_2POW(value);
	writel(value, &clkpwr->hclk_div);
}

static void clkpwr_set_mode(enum clkpwr_mode mode)
{
	struct lpc32xx_clkpwr_regs *clkpwr =
		(struct lpc32xx_clkpwr_regs *)LPC32XX_CLKPWR_BASE;
	u32 value = readl(&clkpwr->pwr_ctrl);

	switch (mode) {
	case CLKPWR_MODE_RUN:
		value |= CLKPWR_SELECT_RUN_MODE;
		break;

	case CLKPWR_MODE_DIRECT:
		value &= ~CLKPWR_SELECT_RUN_MODE;
		break;

	case CLKPWR_MODE_STOP:
		value |= CLKPWR_STOP_MODE_CTRL;
		break;

	default:
		return;
	}

	writel(value, &clkpwr->pwr_ctrl);
}

static void clkpwr_pll_disable(enum clkpwr_pll pll)
{
	struct lpc32xx_clkpwr_regs *clkpwr =
		(struct lpc32xx_clkpwr_regs *)LPC32XX_CLKPWR_BASE;
	u32 value;

	switch (pll) {
	case CLKPWR_PLL_PLL397:
		value = readl(&clkpwr->pll397_ctrl);
		value |= CLKPWR_PLL397_CTRL_DISABLE;
		writel(value, &clkpwr->pll397_ctrl);
		break;

	case CLKPWR_PLL_HCLK:
		value = readl(&clkpwr->hclk_pll_ctrl);
		value &= ~CLKPWR_HCLKPLL_POWER_UP;
		writel(value, &clkpwr->hclk_pll_ctrl);
		break;

	case CLKPWR_PLL_USB:
		value = readl(&clkpwr->usb_ctrl);
		value &= ~CLKPWR_USBCTRL_PLL_PWRUP;
		writel(value, &clkpwr->usb_ctrl);
		break;

	default:
		break;
	}
}

static void clkpwr_mainosc_setup(u32 cap, bool enable)
{
	struct lpc32xx_clkpwr_regs *clkpwr =
		(struct lpc32xx_clkpwr_regs *)LPC32XX_CLKPWR_BASE;
	u32 value;

	value = readl(&clkpwr->main_osc_ctrl);
	value &= ~(CLKPWR_MOSC_CTRL_CAP_MASK | CLKPWR_MOSC_CTRL_DISABLE);

	value |= CLKPWR_MOSC_CTRL_CAP(cap);

	if (!enable)
		value |= CLKPWR_MOSC_CTRL_DISABLE;

	writel(value, &clkpwr->main_osc_ctrl);
}

static void clkpwr_sysclk_setup(enum clkpwr_osc osc, u32 bp)
{
	struct lpc32xx_clkpwr_regs *clkpwr =
		(struct lpc32xx_clkpwr_regs *)LPC32XX_CLKPWR_BASE;
	u32 value;

	value = readl(&clkpwr->sysclk_ctrl);
	value &= ~(CLKPWR_SYSCTRL_BP_MASK | CLKPWR_SYSCTRL_USEPLL397);
	value |= CLKPWR_SYSCTRL_BP_TRIG(bp);

	if (osc == CLKPWR_OSC_PLL397)
		value |= CLKPWR_SYSCTRL_USEPLL397;

	writel(value, &clkpwr->sysclk_ctrl);
}

static void clkpwr_pll397_setup(bool bypass, u32 bias, bool pll)
{
	struct lpc32xx_clkpwr_regs *clkpwr =
		(struct lpc32xx_clkpwr_regs *)LPC32XX_CLKPWR_BASE;
	u32 value;

	value = readl(&clkpwr->pll397_ctrl);
	value &= ~(CLKPWR_PLL397_CTRL_BYPASS | CLKPWR_PLL397_CTRL_BIAS_MASK |
			CLKPWR_PLL397_CTRL_DISABLE);

	if (bypass)
		value |= CLKPWR_PLL397_CTRL_BYPASS;

	value |= bias;

	if (pll)
		value |= CLKPWR_PLL397_CTRL_DISABLE;

	writel(value, &clkpwr->pll397_ctrl);
}

static u32 clkpwr_check_pll_setup(u32 input, struct clkpwr_pll_setup *setup)
{
	u32 mode = (setup->bypass << 2) | (setup->direct << 1) |
		(setup->fdbk << 0);
	u64 freq = 0;
	u64 cco = 0;
	u64 ref = 0;
	u64 p, n, m;

	p = setup->pllp;
	n = setup->plln;
	m = setup->pllm;

	switch (mode) {
	case 0:
		freq = m * input;
		do_div(freq, 2 * p * n);
		cco = m * input;
		do_div(cco, n);
		ref = input;
		do_div(ref, n);
		break;

	case 1:
		freq = m * input;
		do_div(freq, n);
		cco = m * input;
		do_div(cco, 2 * p * n);
		ref = input;
		do_div(ref, n);
		break;

	case 2:
	case 3:
		freq = m * input;
		do_div(freq, n);
		cco = freq;
		ref = input;
		do_div(ref, n);
		break;

	case 4:
	case 5:
		freq = input;
		do_div(freq, 2 * p);
		cco = 156000000;
		ref = 1000000;
		break;

	case 6:
	case 7:
		freq = input;
		cco = 156000000;
		ref = 1000000;
		break;
	}

	if ((cco < 156000000) || (cco > 320000000))
		freq = 0;

	if ((ref < 1000000) || (ref > 27000000))
		freq = 0;

	return freq;
}

static u32 pll_postdivs[4] = { 1, 2, 4, 8 };

static s32 clkpwr_abs(s32 v1, s32 v2)
{
	if (v1 > v2)
		return v1 - v2;

	return v2 - v1;
}

static u32 clkpwr_find_pll_config(u32 input, u32 target, s32 tol,
		struct clkpwr_pll_setup *setup)
{
	bool flag = false;
	u32 clkout = 0;
	u32 ret = 0;
	u32 m, n, p;
	u64 lfreq;
	u32 freq;

	lfreq = (u64)target * (u64)tol;
	do_div(lfreq, 1000);
	freq = (u32)lfreq;

	if (clkpwr_abs(input, target) < freq) { /* direct bypass mode */
		setup->analog = 0;
		setup->bypass = 1;
		setup->direct = 1;
		setup->fdbk = 1;
		setup->pllp = pll_postdivs[0];
		setup->plln = 1;
		setup->pllm = 1;

		clkout = clkpwr_check_pll_setup(input, setup);
	} else if (target < input) { /* bypass mode */
		setup->analog = 0;
		setup->bypass = 1;
		setup->direct = 0;
		setup->fdbk = 1;
		setup->plln = 1;
		setup->pllm = 1;

		for (p = 0; p <= 3; p++) {
			setup->pllp = pll_postdivs[p];
			clkout = clkpwr_check_pll_setup(input, setup);
			if (clkpwr_abs(target, clkout) <= freq) {
				flag = true;
				break;
			}
		}
	}

	if (!flag) { /* direct mode */
		setup->analog = 1;
		setup->bypass = 0;
		setup->direct = 1;
		setup->fdbk = 0;
		setup->pllp = pll_postdivs[0];

		for (m = 1; m <= 256; m++) {
			for (n = 1; n <= 4; n++) {
				setup->plln = n;
				setup->pllm = m;

				clkout = clkpwr_check_pll_setup(input, setup);
				if (clkpwr_abs(target, clkout) <= freq) {
					flag = true;
					break;
				}
			}

			if (flag)
				break;
		}
	}

	/* integer mode */
	if (!flag) {
		setup->analog = 1;
		setup->bypass = 0;
		setup->direct = 0;
		setup->fdbk = 1;

		for (m = 1; m <= 256; m++) {
			for (n = 1; n <= 4; n++) {
				for (p = 0; p < 4; p++) {
					setup->pllp = pll_postdivs[p];
					setup->plln = n;
					setup->pllm = m;

					clkout = clkpwr_check_pll_setup(input, setup);
					if (clkpwr_abs(target, clkout) <= freq) {
						flag = true;
						break;
					}
				}

				if (flag)
					break;
			}

			if (flag)
				break;
		}
	}

	/* try non-integer mode */
	if (!flag) {
		setup->analog = 1;
		setup->bypass = 0;
		setup->direct = 0;
		setup->fdbk = 0;

		for (m = 1; m <= 256; m++) {
			for (n = 1; n <= 4; n++) {
				for (p = 0; p < 4; p++) {
					setup->pllp = pll_postdivs[p];
					setup->plln = n;
					setup->pllm = m;

					clkout = clkpwr_check_pll_setup(input, setup);
					if (clkpwr_abs(target, clkout) <= freq) {
						flag = true;
						break;
					}
				}

				if (flag)
					break;
			}

			if (flag)
				break;
		}
	}

	if (flag)
		ret = clkout;

	return ret;
}

static u32 hclk_divs[4] = { 1, 2, 4, 4 };

static enum clkpwr_osc clkpwr_get_osc(void)
{
	struct lpc32xx_clkpwr_regs *clkpwr =
		(struct lpc32xx_clkpwr_regs *)LPC32XX_CLKPWR_BASE;
	enum clkpwr_osc ret = CLKPWR_OSC_MAIN;
	u32 value;

	value = readl(&clkpwr->sysclk_ctrl);

	if (value & CLKPWR_SYSCTRL_SYSCLKMUX)
		ret = CLKPWR_OSC_PLL397;

	return ret;
}

static u32 clkpwr_pll_rate(u32 clkrate, u32 value)
{
	struct clkpwr_pll_setup setup;

	setup.analog = 0;
	setup.bypass = 0;
	setup.direct = 0;
	setup.fdbk = 0;

	if (value & CLKPWR_HCLKPLL_CCO_BYPASS)
		setup.bypass = 1;

	if (value & CLKPWR_HCLKPLL_POSTDIV_BYPASS)
		setup.direct = 1;

	if (value & CLKPWR_HCLKPLL_FDBK_SEL_FCLK)
		setup.fdbk = 1;

	setup.pllp = pll_postdivs[(value >> 11) & 0x3];
	setup.pllm = 1 + ((value >> 1) & 0xff);
	setup.plln = 1 + ((value >> 9) & 0x03);

	return clkpwr_check_pll_setup(clkrate, &setup);
}

static u32 clkpwr_get_base_clock_rate(enum clkpwr_clk clock)
{
	struct lpc32xx_clkpwr_regs *clkpwr =
		(struct lpc32xx_clkpwr_regs *)LPC32XX_CLKPWR_BASE;
	u32 sysclk, hclk, ddrclk, pclk, armclk;
	u32 hclk_div = 0;
	u32 value;
	u32 ret;

	value = readl(&clkpwr->sysclk_ctrl);

	if (value & CLKPWR_SYSCTRL_USEPLL397)
		sysclk = CLOCK_OSC_FREQ * 397;
	else
		sysclk = MAIN_OSC_FREQ;

	value = readl(&clkpwr->sdram_clk_ctrl);

	if (value & CLKPWR_SDRCLK_USE_DDR) {
		value = readl(&clkpwr->hclk_div);

		if (value & CLKPWR_HCLKDIV_DDRCLK_NORM)
			hclk_div = 1;
		else if (value & CLKPWR_HCLKDIV_DDRCLK_HALF)
			hclk_div = 2;
	} else {
		value = readl(&clkpwr->hclk_div);
		value &= CLKPWR_HCLKDIV_DIV_2POW(0x3);
		hclk_div = hclk_divs[value] - 1;
	}

	value = readl(&clkpwr->pwr_ctrl);

	if (value & CLKPWR_SELECT_RUN_MODE) {
		u32 div;

		value = readl(&clkpwr->hclk_pll_ctrl);
		hclk = clkpwr_pll_rate(sysclk, value);
		ddrclk = hclk;

		value = readl(&clkpwr->hclk_div);
		div = 1 + ((value >> 2) & 0x1f);
		pclk = hclk / div;

		hclk = hclk / hclk_divs[CLKPWR_HCLKDIV_DIV_2POW(div)];
		armclk = hclk;
	} else {
		ddrclk = sysclk;
		pclk = sysclk;
		hclk = sysclk;
		armclk = sysclk;
	}

	ddrclk = ddrclk / (hclk_div + 1);

	value = readl(&clkpwr->pwr_ctrl);

	if (value & CLKPWR_PWR_CTRL_FORCE_PCLK) {
		hclk = pclk;
		armclk = pclk;
	}

	switch (clock) {
	case CLKPWR_CLK_MAINOSC:
		ret = MAIN_OSC_FREQ;
		break;

	case CLKPWR_CLK_RTC:
		ret = CLOCK_OSC_FREQ;
		break;

	case CLKPWR_CLK_SYS:
		ret = sysclk;
		break;

	case CLKPWR_CLK_ARM:
		ret = armclk;
		break;

	case CLKPWR_CLK_HCLK:
	case CLKPWR_CLK_MSSD:
		ret = hclk;
		break;

	case CLKPWR_CLK_PERIPH:
		ret = pclk;
		break;

	case CLKPWR_CLK_DDR:
		ret = ddrclk;
		break;

	case CLKPWR_CLK_USB:
	case CLKPWR_CLK_48M:
	default:
		ret = 0;
		break;
	}

	return ret;
}

static u32 clkpwr_hclkpll_setup(struct clkpwr_pll_setup *setup)
{
	struct lpc32xx_clkpwr_regs *clkpwr =
		(struct lpc32xx_clkpwr_regs *)LPC32XX_CLKPWR_BASE;
	u32 value = 0;
	u32 div = 0;
	u32 clkrate;

	if (setup->analog)
		value |= CLKPWR_HCLKPLL_POWER_UP;

	if (setup->bypass)
		value |= CLKPWR_HCLKPLL_CCO_BYPASS;

	if (setup->direct)
		value |= CLKPWR_HCLKPLL_POSTDIV_BYPASS;

	if (setup->fdbk)
		value |= CLKPWR_HCLKPLL_FDBK_SEL_FCLK;

	switch (setup->pllp) {
	case 1:
		div = 0;
		break;

	case 2:
		div = 1;
		break;

	case 4:
		div = 2;
		break;

	case 8:
		div = 3;
		break;

	default:
		return 0;
	}

	value |= CLKPWR_HCLKPLL_POSTDIV_2POW(div);
	value |= CLKPWR_HCLKPLL_PREDIV_PLUS1(setup->plln - 1);
	value |= CLKPWR_HCLKPLL_PLLM(setup->pllm - 1);

	writel(value, &clkpwr->hclk_pll_ctrl);

	clkrate = clkpwr_get_base_clock_rate(CLKPWR_CLK_SYS);
	return clkpwr_check_pll_setup(clkrate, setup);
}

static bool clkpwr_is_pll_locked(enum clkpwr_pll pll)
{
	struct lpc32xx_clkpwr_regs *clkpwr
		= (struct lpc32xx_clkpwr_regs *)LPC32XX_CLKPWR_BASE;
	bool locked = false;
	u32 value;

	switch (pll) {
	case CLKPWR_PLL_PLL397:
		value = readl(&clkpwr->pll397_ctrl);
		if (value & CLKPWR_PLL397_CTRL_STS)
			locked = true;
		break;

	case CLKPWR_PLL_HCLK:
		value = readl(&clkpwr->hclk_pll_ctrl);
		if (value & CLKPWR_HCLKPLL_PLL_STS)
			locked = true;
		else if ((value & CLKPWR_HCLKPLL_POWER_UP) == 0)
			locked = true;
		break;

	case CLKPWR_PLL_USB:
		value = readl(&clkpwr->usb_ctrl);
		if (value & CLKPWR_USBCTRL_PLL_STS)
			locked = true;
		else if ((value & CLKPWR_USBCTRL_PLL_PWRUP) == 0)
			locked = true;
		break;

	default:
		break;
	}

	return locked;
}

static void clkpwr_force_arm_hclk_to_pclk(bool force)
{
	struct lpc32xx_clkpwr_regs *clkpwr =
		(struct lpc32xx_clkpwr_regs *)LPC32XX_CLKPWR_BASE;
	u32 value;

	value = readl(&clkpwr->pwr_ctrl);
	value &= ~CLKPWR_PWR_CTRL_FORCE_PCLK;

	if (force)
		value |= CLKPWR_PWR_CTRL_FORCE_PCLK;

	writel(value, &clkpwr->pwr_ctrl);
}

#define CPU_CLOCK_RATE 208000000
#define HCLK_DIVIDER 2
#define PCLK_DIVIDER 16

static void setup_clocks(u32 clkrate, u32 hdiv, u32 pdiv)
{
	struct clkpwr_pll_setup pllcfg;

	clkpwr_set_hclk_divs(CLKPWR_HCLKDIV_DDRCLK_STOP, 1, 2);
	clkpwr_set_mode(CLKPWR_MODE_DIRECT);
	clkpwr_pll_disable(CLKPWR_PLL_HCLK);
	__udelay(2);

	if (clkpwr_get_osc() == CLKPWR_OSC_PLL397) {
		clkpwr_mainosc_setup(0, true);
		__udelay(100);

		clkpwr_sysclk_setup(CLKPWR_OSC_MAIN, 0x50);
		clkpwr_pll397_setup(false, 0, false);
	} else {
		clkpwr_sysclk_setup(CLKPWR_OSC_MAIN, 0x50);
	}

	clkrate = clkpwr_find_pll_config(MAIN_OSC_FREQ, clkrate, 5, &pllcfg);
	if (clkrate) {
		clkpwr_hclkpll_setup(&pllcfg);

		while (!clkpwr_is_pll_locked(CLKPWR_PLL_HCLK));

		clkpwr_set_hclk_divs(CLKPWR_HCLKDIV_DDRCLK_STOP, pdiv, hdiv);

		clkpwr_force_arm_hclk_to_pclk(false);
		clkpwr_set_mode(CLKPWR_MODE_RUN);
	}
}

#define SDRAM_PERFORMANCE_MODE 1

#define SDRAM_BANKS 2
#define SDRAM_COLS 10
#define SDRAM_ROWS 13

#define SDRAM_REFRESH_INTERVAL 128200
#define SDRAM_CAS_LATENCY 5
#define SDRAM_RAS_LATENCY 2

static u32 ddr_get_bankmask(u32 bankshift, u32 a1, u32 a0)
{
	u32 offset = 0;

	if (bankshift & 1)
		offset = (a0 << 1) | a1;
	else
		offset = (a1 << 1) | a0;

	return offset << bankshift;
}

#define SDRAM_MODE_WORD 0x61
#define SDRAM_EXT_MODE_WORD 0x00
#define SDRAM_EXT_MODE_BB ddr_get_bankmask(bankshift, 0, 1)

static struct sdram_cfg_map {
	u8 banks;
	u8 rows;
	u8 cols;
	u8 config;
} sdram_map[] = {
	{ 2, 13, 10, 0x11 },
	{ 2, 13, 11, 0x10 },
	{ 2, 13,  9, 0x0d },
	{ 2, 12,  9, 0x09 },
	{ 2, 12, 10, 0x08 },
	{ 2, 12,  8, 0x05 },
	{ 1, 11,  8, 0x01 },
	{ 1, 11,  9, 0x00 },
};

#define SDRAM_LP_MASK_BIT (1 << 5)

static u32 ddr_find_config(u32 *modeshift, u32 *bankshift)
{
	const u32 bus32 = 0;
	unsigned int i;
	u32 ret = 0;

	for (i = 0; i < ARRAY_SIZE(sdram_map); i++) {
		if ((sdram_map[i].banks == SDRAM_BANKS) &&
		    (sdram_map[i].rows == SDRAM_ROWS) &&
		    (sdram_map[i].cols == SDRAM_COLS)) {
			ret = sdram_map[i].config;
#ifndef SDRAM_PERFORMANCE_MODE
			ret |= SDRAM_LP_MASK_BIT;
#endif
		}
	}

#ifdef SDRAM_PERFORMANCE_MODE
	*modeshift = SDRAM_COLS + bus32 + 1 + SDRAM_BANKS;
	*bankshift = SDRAM_COLS + bus32 + 1;
#else
	*modeshift = SDRAM_COLS + bus32 + 1;
	*bankshift = SDRAM_COLS + SDRAM_ROWS + bus32 + 1;
#endif

	return ret;
}

#define EMC_DYN_READ_CFG_SDR_CLK_NODLY_CMD_DEL 1
#define EMC_DYN_READ_CFG_SDR_READCAP_POS_POL (1 << 4)
#define EMC_DYN_READ_CFG_DDR_CLK_NODLY_CMD_DEL (1 << 8)
#define EMC_DYN_READ_CFG_DDR_READCAP_POS_POL (1 << 12)
#define EMC_DYN_CFG_DEV_LP_DDR_SDRAM 6

#define CLKPWR_SDRCLK_SLOWSLEW_CLK (1 << 22)
#define CLKPWR_SDRCLK_SLOWSLEW (1 << 21)
#define CLKPWR_SDRCLK_SLOWSLEW_DAT (1 << 20)

static void ddr_clock_resync(u32 cfg)
{
	struct lpc32xx_clkpwr_regs *clkpwr =
		(struct lpc32xx_clkpwr_regs *)LPC32XX_CLKPWR_BASE;
	struct lpc32xx_emc_regs *emc =
		(struct lpc32xx_emc_regs *)LPC32XX_EMC_BASE;
	u32 value;

	value = readl(&clkpwr->sdram_clk_ctrl);
	value |= CLKPWR_SDRCLK_SW_DDR_RESET;
	writel(value, &clkpwr->sdram_clk_ctrl);

	value = readl(&clkpwr->sdram_clk_ctrl);
	value &= ~CLKPWR_SDRCLK_SW_DDR_RESET;
	writel(value, &clkpwr->sdram_clk_ctrl);

	value = (cfg << 7) | EMC_DYN_CFG_DEV_LP_DDR_SDRAM;
	writel(value, &emc->dyn_config0);
}

static void ddr_if_init(u32 cfg)
{
	struct lpc32xx_clkpwr_regs *clkpwr =
		(struct lpc32xx_clkpwr_regs *)LPC32XX_CLKPWR_BASE;
	struct lpc32xx_emc_regs *emc =
		(struct lpc32xx_emc_regs *)LPC32XX_EMC_BASE;
	unsigned int count = 0;
	unsigned int i;
	u32 value;

	value = readl(&clkpwr->hclk_div);
	value &= ~(0x3 << 7);
	value |= CLKPWR_HCLKDIV_DDRCLK_NORM;
	writel(value, &clkpwr->hclk_div);

	ddr_clock_resync(cfg);

	value = readl(&clkpwr->sdram_clk_ctrl);
	value |= CLKPWR_SDRCLK_USE_CAL | CLKPWR_SDRCLK_SENS_FACT(7) |
		CLKPWR_SDRCLK_DQS_DLY(15);
	writel(value, &clkpwr->sdram_clk_ctrl);

	for (i = 0; i < 10; i++) {
		value = readl(&clkpwr->sdram_clk_ctrl);
		value |= CLKPWR_SDRCLK_DO_CAL;
		writel(value, &clkpwr->sdram_clk_ctrl);

		value = readl(&clkpwr->sdram_clk_ctrl);
		value &= ~CLKPWR_SDRCLK_DO_CAL;
		writel(value, &clkpwr->sdram_clk_ctrl);

		__udelay(25);

		count += readl(&clkpwr->ddr_lap_count);
	}

	writel(count / 10, &clkpwr->ddr_lap_nom);

	value = readl(&clkpwr->sdram_clk_ctrl);
	value |= CLKPWR_SDRCLK_CAL_ON_RTC;
	value &= ~CLKPWR_SDRCLK_USE_CAL;
	writel(value, &clkpwr->sdram_clk_ctrl);

	value = ((SDRAM_CAS_LATENCY & 0xf) << 7) | (SDRAM_RAS_LATENCY & 0xf);
	writel(value, &emc->dyn_rascas0);

	value = EMC_DYN_READ_CFG_SDR_CLK_NODLY_CMD_DEL |
		EMC_DYN_READ_CFG_SDR_READCAP_POS_POL |
		EMC_DYN_READ_CFG_DDR_CLK_NODLY_CMD_DEL |
		EMC_DYN_READ_CFG_DDR_READCAP_POS_POL;
	writel(value, &emc->dyn_read_cfg);

	value = readl(&clkpwr->sdram_clk_ctrl);
	value &= ~(CLKPWR_SDRCLK_SLOWSLEW_CLK | CLKPWR_SDRCLK_SLOWSLEW |
			CLKPWR_SDRCLK_SLOWSLEW_DAT);
	writel(value, &clkpwr->sdram_clk_ctrl);
}

#define SDRAM_TRP 1
#define SDRAM_TRAS 4
#define SDRAM_TSREX 0x48
#define SDRAM_TWR 1
#define SDRAM_TRC 7
#define SDRAM_TRFC 7
#define SDRAM_TXSR 7
#define SDRAM_TRRD 1
#define SDRAM_TMRD 2
#define SDRAM_TCDLR 2

#define TRP(n) ((n) & 0xf)
#define TRAS(n) ((n) & 0xf)
#define TSREX(n) ((n) & 0x7f)
#define TWR(n) ((n) & 0xf)
#define TRC(n) ((n) & 0x1f)
#define TRFC(n) ((n) & 0x1f)
#define TXSR(n) ((n) & 0xff)
#define TRRD(n) ((n) & 0xf)
#define TMRD(n) ((n) & 0xf)
#define TCDLR(n) ((n) & 0xf)

static void ddr_adjust_timing(unsigned int clkrate)
{
	struct lpc32xx_emc_regs *emc =
		(struct lpc32xx_emc_regs *)LPC32XX_EMC_BASE;

	writel(TRP(SDRAM_TRP), &emc->dyn_trp);
	writel(TRAS(SDRAM_TRAS), &emc->dyn_tras);
	writel(TSREX(SDRAM_TSREX), &emc->dyn_tsrex);
	writel(TWR(SDRAM_TWR), &emc->dyn_twr);
	writel(TRC(SDRAM_TRC), &emc->dyn_trc);
	writel(TRFC(SDRAM_TRFC), &emc->dyn_trfc);
	writel(TXSR(SDRAM_TXSR), &emc->dyn_txsr);
	writel(TRRD(SDRAM_TRRD), &emc->dyn_trrd);
	writel(TMRD(SDRAM_TMRD), &emc->dyn_tmrd);
	writel(TCDLR(SDRAM_TCDLR), &emc->dyn_tcdlr);
}

static const u8 dqs2calsen[32] = {
	7, 5, 4, 4, 3, 3, 3, 2, 2, 2, 2, 2, 2, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0
};

static void dqsin_ddr_mod(u32 delay)
{
	struct lpc32xx_clkpwr_regs *clkpwr =
		(struct lpc32xx_clkpwr_regs *)LPC32XX_CLKPWR_BASE;
	u32 value;

	value = readl(&clkpwr->sdram_clk_ctrl);
	value &= ~(CLKPWR_SDRCLK_SENS_FACT(7) | CLKPWR_SDRCLK_DQS_DLY(0x1f));
	value |= CLKPWR_SDRCLK_DQS_DLY(delay);
	value |= CLKPWR_SDRCLK_SENS_FACT(dqs2calsen[delay]);
	writel(value, &clkpwr->sdram_clk_ctrl);
}

/* TODO: improve this memory test */
static int ddr_memtest(unsigned long base, unsigned long size)
{
	unsigned long step = size / 256;
	unsigned long i;
	int ret = 0;

	for (i = 0; i < size; i += step) {
		u32 value = 0xaa551100;

		writel(value, base + i);
		value = readl(base + i);

		if (value != 0xaa551100) {
			ret = 1;
			break;
		}
	}

	return ret;
}

static int ddr_find_dqsin_delay(unsigned long base, unsigned long size)
{
	int start = 0xff;
	int end = 0xff;
	u32 delay = 1;
	int pass = 0;
	int good = 0;

	while (delay < 31) {
		dqsin_ddr_mod(delay);

		if (ddr_memtest(base, size) == 0) {
			if (start == 0xff)
				start = delay;

			pass = 1;
		} else {
			if (pass == 1) {
				end = delay - 1;
				pass = 0;
				good = 1;
			}
		}

		delay++;
	}

	if (good)
		delay = (start + end) / 2;
	else
		delay = 0xf;

	dqsin_ddr_mod(delay);
	return good;
}

static void ddr_st_setup(unsigned int clkrate)
{
	struct lpc32xx_clkpwr_regs *clkpwr =
		(struct lpc32xx_clkpwr_regs *)LPC32XX_CLKPWR_BASE;
	struct lpc32xx_emc_regs *emc =
		(struct lpc32xx_emc_regs *)LPC32XX_EMC_BASE;
	u32 modeshift = 0;
	u32 bankshift = 0;
	u32 value;
	u16 word;
	u32 cfg;

	writel(CLKPWR_SDRCLK_USE_DDR, &clkpwr->sdram_clk_ctrl);
	writel(CLKPWR_SDRCLK_HCLK_DLY(15) | CLKPWR_SDRCLK_USE_DDR, &clkpwr->sdram_clk_ctrl);

	writel(EMC_STC_BLS_EN | EMC_STC_MEMWIDTH_16, &emc->stc[0].config);
	writel(EMC_STC_BLS_EN | EMC_STC_MEMWIDTH_16, &emc->stc[1].config);

	cfg = ddr_find_config(&modeshift, &bankshift);
	if (!cfg) {
		serial_puts("ddr_find_config() failed\n");
		return;
	}

	ddr_if_init(cfg);

	ddr_adjust_timing(clkrate);

	value = EMC_DYN_CLK_ALWAYS_ON | EMC_DYN_CLKEN_ALWAYS_ON;

	/* nop mode */
	writel(value | EMC_DYN_NOP_MODE, &emc->dyn_control);
	__udelay(200);

	/* precharge all command */
	writel(value | EMC_DYN_PALL_MODE, &emc->dyn_control);
	writel(EMC_DYN_REFRESH_IVAL(4), &emc->dyn_refresh);
	__udelay(10);

	/* cmd mode */
	writel(value | EMC_DYN_CMD_MODE, &emc->dyn_control);

	word = readw(0x80000000 + (SDRAM_EXT_MODE_WORD << modeshift) + SDRAM_EXT_MODE_BB);
	__udelay(1);

	word = readw(0x80000000 + (((2 << 7) + SDRAM_MODE_WORD) << modeshift));
	__udelay(1);

	/* precharge all command */
	writel(value | EMC_DYN_PALL_MODE, &emc->dyn_control);
	__udelay(10);
	__udelay(25);

	writel(EMC_DYN_REFRESH_IVAL(clkrate / SDRAM_REFRESH_INTERVAL), &emc->dyn_refresh);

	writel(value | EMC_DYN_CMD_MODE, &emc->dyn_control);
	word = readw(0x80000000 + (SDRAM_MODE_WORD << modeshift));
	__udelay(1);

	writel(EMC_DYN_NORMAL_MODE | EMC_DYN_DIS_MEMCLK_IN_SFRSH, &emc->dyn_control);
	__udelay(10);

	ddr_find_dqsin_delay(0x80000000, 64 << 20);
	__udelay(20); /* Needed delay for system not to hang sporadically */

	value = readl(&clkpwr->sdram_clk_ctrl);
	value |= CLKPWR_SDRCLK_USE_CAL;
	writel(value, &clkpwr->sdram_clk_ctrl);

	(void)word;
}

static void setup_memory(void)
{
	struct lpc32xx_clkpwr_regs *clkpwr =
		(struct lpc32xx_clkpwr_regs *)LPC32XX_CLKPWR_BASE;
	struct lpc32xx_emc_regs *emc =
		(struct lpc32xx_emc_regs *)LPC32XX_EMC_BASE;
	unsigned int rate;

	writel(1, &clkpwr->bootmap);
	writel(0, &clkpwr->sdram_clk_ctrl);

	writel(EMC_CONTROL_DYN_SDRAM_CTRL_EN, &emc->control);
	writel(0, &emc->config);
	writel(0x7ff, &emc->dyn_refresh);

	rate = sys_get_rate(CLKPWR_CLK_DDR);

	ddr_st_setup(rate);

	writel(EMC_AHB_CONTROL_PORTBUFF_EN, &emc->ahb[0].control);
	writel(EMC_AHB_CONTROL_PORTBUFF_EN, &emc->ahb[2].control);
	writel(EMC_AHB_CONTROL_PORTBUFF_EN, &emc->ahb[3].control);
	writel(EMC_AHB_CONTROL_PORTBUFF_EN, &emc->ahb[4].control);

	writel(32, &emc->ahb[0].timeout);
	writel(32, &emc->ahb[2].timeout);
	writel(32, &emc->ahb[3].timeout);
	writel(32, &emc->ahb[4].timeout);

	writel(0x0f, &emc->stc[0].waitwen);
	writel(0x0f, &emc->stc[0].wait0en);
	writel(0x1f, &emc->stc[0].waitrd);
	writel(0x1f, &emc->stc[0].page);
	writel(0x1f, &emc->stc[0].wr);
	writel(0x0f, &emc->stc[0].turn);
}

#if defined(CONFIG_NAND_SPL)
void board_init_f(ulong bootflag)
{
	nand_boot();
}

void hang(void)
{
	serial_puts("### ERROR ### Please RESET the board ###\r\n");

	while (1)
		;
}

void panic(const char *fmt, ...)
{
	hang();
}
#endif
#endif

void s_init(void)
{
#if defined(CONFIG_BOOTSTRAP_BUILD) || defined(CONFIG_NAND_SPL)

#if defined(PTIP_CLASSIC_SETUP_RT_BL)
	setup_rt_bl();
#endif
	timer_init();

	gd = &gdata;
	gd->bd = &bdata;
	gd->baudrate = 115200;

	serial_init();
	setup_gpio();
	setup_clocks(CPU_CLOCK_RATE, HCLK_DIVIDER, PCLK_DIVIDER);
	setup_memory();
#endif
}
