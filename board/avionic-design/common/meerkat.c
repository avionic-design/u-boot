/*
 * (C) Copyright 2014, NVIDIA Corporation <www.nvidia.com>
 * (C) Copyright 2015, Avionic Design <www.avionic-design.de>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include <common.h>
#include <errno.h>
#include <power/as3722.h>

#include <asm/arch/gpio.h>
#include <asm/arch/pinmux.h>
#include "pinmux-config-meerkat.h"

/*
 * Routine: pinmux_init
 * Description: Do individual peripheral pinmux configs
 */
void pinmux_init(void)
{
	pinmux_set_tristate_input_clamping();

	pinmux_config_pingrp_table(meerkat_pingrps,
				   ARRAY_SIZE(meerkat_pingrps));

	pinmux_config_drvgrp_table(meerkat_drvgrps,
				   ARRAY_SIZE(meerkat_drvgrps));
}

#ifdef CONFIG_PCI_TEGRA
int tegra_pcie_board_init(void)
{
	struct udevice *pmic;
	int err;

	err = as3722_init(&pmic, 4, 0x40);
	if (err) {
		error("failed to initialize AS3722 PMIC: %d\n", err);
		return err;
	}

	err = as3722_sd_enable(pmic, 4);
	if (err < 0) {
		error("failed to enable SD4: %d\n", err);
		return err;
	}

	err = as3722_sd_set_voltage(pmic, 4, 0x24);
	if (err < 0) {
		error("failed to set SD4 voltage: %d\n", err);
		return err;
	}

	err = as3722_gpio_configure(pmic, 1, AS3722_GPIO_OUTPUT_VDDH |
					     AS3722_GPIO_INVERT);
	if (err < 0) {
		error("failed to configure GPIO#1 as output: %d\n", err);
		return err;
	}

	err = as3722_gpio_direction_output(pmic, 2, 1);
	if (err < 0) {
		error("failed to set GPIO#2 high: %d\n", err);
		return err;
	}

	return 0;
}
#endif /* PCI */

int ft_board_setup(void *blob, bd_t *bd)
{
	gpu_enable_node(blob, "/gpu@0,57000000");

	return 0;
}