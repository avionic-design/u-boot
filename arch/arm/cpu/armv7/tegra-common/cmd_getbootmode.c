/*
 * (C) Copyright 2015
 * Avionic Design GmbH <www.avionic-design.de>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/arch/tegra.h>
#include <asm/arch-tegra/pmc.h>

#define RECOVERY_MODE   (1<<31)
#define BOOTLOADER_MODE (1<<30)

#define RECOVERY_STR    "recovery"
#define BOOTLOADER_STR  "bootloader"
#define DEFAULT_STR     "boot"

static int do_getbootmode(cmd_tbl_t *cmdtp, int flag, int argc,
		       char * const argv[])
{
	struct pmc_ctlr *pmc = (struct pmc_ctlr *)NV_PA_PMC_BASE;
	const char *str = DEFAULT_STR;

	if (pmc->pmc_scratch0 & RECOVERY_MODE)
		str = RECOVERY_STR;
	else if (pmc->pmc_scratch0 & BOOTLOADER_MODE)
		str = BOOTLOADER_STR;
	printf("Boot mode: %s\n", str);
	setenv("boot_mode", str);

	return 0;
}

U_BOOT_CMD(
	getbootmode, 1, 0, do_getbootmode,
	"Get Tegra boot mode",
	""
);
