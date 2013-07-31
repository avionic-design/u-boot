/*
 *  (C) Copyright 2013
 *  Avionic Design GmbH <www.avionic-design.de>
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
#include <asm/arch/pinmux.h>
#include <asm/arch/gp_padctrl.h>
#include <asm/arch/gpio.h>
#include <asm/gpio.h>
#include "pinmux-config-tamonten-ng.h"
#include <i2c.h>

#define PMU_I2C_ADDRESS		0x2D
#define MAX_I2C_RETRY		3

#define PMU_REG_LDO1		0x30
#define PMU_REG_LDO2		0x31
#define PMU_REG_LDO5		0x32
#define PMU_REG_LDO8		0x33
#define PMU_REG_LDO7		0x34
#define PMU_REG_LDO6		0x35
#define PMU_REG_LDO4		0x36
#define PMU_REG_LDO3		0x37

#define PMU_REG_LDO_OFF		0
#define PMU_REG_LDO_HIGH_POWER	1
#define PMU_REG_LDO_LOW_POWER	3

/* Voltage selection for the LDOs with 50mV resolution */
#define PMU_REG_LDO_SEL_50(mV)	((((mV - 1000) / 50) + 4) << 2)
/* Voltage selection for the LDOs with 100mV resolution */
#define PMU_REG_LDO_SEL_100(mV)	((((mV - 1000) / 100) + 2) << 2)

#define PMU_REG_LDO_50(st, mV)	(PMU_REG_LDO_##st | PMU_REG_LDO_SEL_50(mV))
#define PMU_REG_LDO_100(st, mV)	(PMU_REG_LDO_##st | PMU_REG_LDO_SEL_50(mV))

#define PMU_LDO1(st, mV)	PMU_REG_LDO_50(st, mV)
#define PMU_LDO2(st, mV)	PMU_REG_LDO_50(st, mV)
#define PMU_LDO5(st, mV)	PMU_REG_LDO_100(st, mV)
#define PMU_LDO8(st, mV)	PMU_REG_LDO_100(st, mV)
#define PMU_LDO7(st, mV)	PMU_REG_LDO_100(st, mV)
#define PMU_LDO6(st, mV)	PMU_REG_LDO_100(st, mV)
#define PMU_LDO4(st, mV)	PMU_REG_LDO_50(st, mV)
#define PMU_LDO3(st, mV)	PMU_REG_LDO_100(st, mV)

void pinmux_init(void)
{
	pinmux_config_table(tamonten_ng_pinmux_common,
			    ARRAY_SIZE(tamonten_ng_pinmux_common));
	pinmux_config_table(unused_pins_lowpower,
			    ARRAY_SIZE(unused_pins_lowpower));

	/* Initialize any non-default pad configs (APB_MISC_GP regs) */
	padgrp_config_table(tamonten_ng_padctrl,
			    ARRAY_SIZE(tamonten_ng_padctrl));
}


void pmu_write(uchar reg, uchar data)
{
	int i;
	i2c_set_bus_num(0);	/* PMU is on bus 0 */
	for (i = 0; i < MAX_I2C_RETRY; ++i) {
		if (i2c_write(PMU_I2C_ADDRESS, reg, 1, &data, 1))
			udelay(100);
		else
			break;
	}
}

void power_init_board(void)
{
	/* Set the periphs in reset */
	gpio_request(GPIO_PI4, "nRST_PERIPH");
	gpio_direction_output(GPIO_PI4, 0);

	/* Disable the PCIe power */
	gpio_request(GPIO_PI4, "EN_3V3_PEX_HVDD");
	gpio_direction_output(GPIO_PT3, 0);

	/* Disable the SATA power */
	gpio_request(GPIO_PI4, "EN_3V3_SATA_HVDD");
	gpio_direction_output(GPIO_PK3, 0);

	/* Disable LDO1 for PCIe and SATA */
	pmu_write(PMU_REG_LDO1, PMU_LDO1(OFF, 1050));

	/* Turn on the alive signal */
	gpio_request(GPIO_PV2, "ALIVE");
	gpio_direction_output(GPIO_PV2, 1);

        /* Wait for the periph to finish their reset */
	udelay(1000);
	gpio_direction_output(GPIO_PI4, 1);
}

#if defined(CONFIG_TEGRA_MMC)
/*
 * Do I2C/PMU writes to bring up SD card bus power
 *
 */
void board_sdmmc_voltage_init(void)
{
	/* Enable LDO5 with 3.3v for SDMMC3 */
	pmu_write(PMU_REG_LDO5, PMU_LDO5(HIGH_POWER, 3300));

	/* Switch the power on */
	gpio_request(GPIO_PV2, "EN_3V3_EMMC");
	gpio_direction_output(GPIO_PJ2, 1);
}

/*
 * Routine: pin_mux_mmc
 * Description: setup the MMC muxes, power rails, etc.
 */
void pin_mux_mmc(void)
{
	/*
	 * NOTE: We don't do mmc-specific pin muxes here.
	 * They were done globally in pinmux_init().
	 */

	/* Bring up the SDIO1 power rail */
	board_sdmmc_voltage_init();
}
#endif	/* MMC */
