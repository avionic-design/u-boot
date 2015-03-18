/*
 * (C) Copyright 2014, NVIDIA Corporation <www.nvidia.com>
 * (C) Copyright 2015, Avionic Design <www.avionic-design.de>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include <common.h>
#include <errno.h>
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
