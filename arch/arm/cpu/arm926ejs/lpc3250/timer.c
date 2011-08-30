#include <common.h>

#include <lpc3250.h>

void reset_timer(void)
{
	unsigned int clkdlycnt, tbaseclk;

	/* Reset timer */
	TIMER_CNTR0->tcr = TIMER_CNTR_TCR_RESET;
	TIMER_CNTR0->tcr = 0;
	TIMER_CNTR0->tc = 0;

	/* Clear and enable match function */
	TIMER_CNTR0->ir = TIMER_CNTR_MTCH_BIT(0);

	/* Count mode is PCLK edge */
	TIMER_CNTR0->ctcr = TIMER_CNTR_SET_MODE(TIMER_CNTR_CTCR_TIMER_MODE);

	/* Set prescale counter value for a 1mS tick */
	tbaseclk = sys_get_rate(CLKPWR_PERIPH_CLK);
	clkdlycnt = (tbaseclk / CONFIG_SYS_HZ);
	TIMER_CNTR0->pr = clkdlycnt - 1;

	/* Enable the timer */
	TIMER_CNTR0->tcr = TIMER_CNTR_TCR_EN;
}

ulong get_timer(ulong base)
{
	ulong tcr = TIMER_CNTR0->tc;
	return tcr - base;
}

int timer_init(void)
{
	/* Enable timer system clock */
	CLKPWR->clkpwr_timers_pwms_clk_ctrl_1 |=
		CLKPWR_TMRPWMCLK_TIMER0_EN;

	reset_timer();
	return 0;
}

void __udelay(unsigned long usec)
{
	unsigned long tbaseclk;

	/* Enable timer system clock */
	CLKPWR->clkpwr_timers_pwms_clk_ctrl_1 |=
		CLKPWR_TMRPWMCLK_TIMER1_EN;

	/* Reset timer */
	TIMER_CNTR1->tcr = TIMER_CNTR_TCR_RESET;
	TIMER_CNTR1->tcr = 0;
	TIMER_CNTR1->tc = 0;
	TIMER_CNTR1->pr = 0;

	/* Clear and enable match function */
	TIMER_CNTR1->ir = TIMER_CNTR_MTCH_BIT(0);

	/* Count mode is PCLK edge */
	TIMER_CNTR1->ctcr = TIMER_CNTR_SET_MODE(TIMER_CNTR_CTCR_TIMER_MODE);

	/* Set prescale counter value for a 1uS tick */
	tbaseclk = sys_get_rate(CLKPWR_PERIPH_CLK);
	tbaseclk = (tbaseclk / 1000000) - 1;
	TIMER_CNTR1->pr = tbaseclk;

	/* Set match for number of usecs */
	TIMER_CNTR1->mr[0] = usec;

	/* Interrupt on match 0 */
	TIMER_CNTR1->mcr = TIMER_CNTR_MCR_MTCH(0);

	/* Enable the timer */
	TIMER_CNTR1->tcr = TIMER_CNTR_TCR_EN;

	/* Loop until match occurs */
	while ((TIMER_CNTR1->ir & TIMER_CNTR_MTCH_BIT(0)) == 0);

	/* Stop timer */
	TIMER_CNTR1->tcr = 0;

	/* Disable timer system clock */
	CLKPWR->clkpwr_timers_pwms_clk_ctrl_1 &=
		~CLKPWR_TMRPWMCLK_TIMER1_EN;
}
