/*
 * (C) Copyright 2014-2016 Avionic Design GmbH
 *
 * SPDX-License-Identifier:     GPL-2.0
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include <linux/sizes.h>

/* enable PMIC */
#define CONFIG_AS3722_POWER

#include "tegra124-common.h"

/* High-level configuration options */
#define CONFIG_TEGRA_BOARD_STRING	"Avionic-Design Kein Baseboard"

/* Board-specific serial config */
#define CONFIG_TEGRA_ENABLE_UARTD
#define CONFIG_SYS_NS16550_COM1		NV_PA_APB_UARTD_BASE

/* I2C */
#define CONFIG_SYS_I2C_TEGRA

/* SD/MMC */
#define CONFIG_MMC
#define CONFIG_GENERIC_MMC
#define CONFIG_TEGRA_MMC
#define CONFIG_CMD_GPT
#define CONFIG_RANDOM_UUID

/* Environment in eMMC, at the end of 2nd "boot sector" */
#define CONFIG_ENV_IS_IN_MMC
#define CONFIG_ENV_OFFSET		(-CONFIG_ENV_SIZE)
#define CONFIG_SYS_MMC_ENV_DEV		0
#define CONFIG_SYS_MMC_ENV_PART		2

/* USB Host support */
#define CONFIG_USB_EHCI
#define CONFIG_USB_EHCI_TEGRA

/* USB networking support */
#define CONFIG_USB_HOST_ETHER
#define CONFIG_USB_ETHER_RNDIS
#define CONFIG_USB_ETHER_ASIX
#define CONFIG_USB_ETHER_ASIX88179
#define CONFIG_USB_ETHER_MCS7830
#define CONFIG_USB_ETHER_SMSC95XX

/* PCI host support */
#define CONFIG_PCI
#define CONFIG_PCI_PNP
#define CONFIG_CMD_PCI

#define BOARD_EXTRA_ENV_SETTINGS					\
	"get_mmc_blocks="						\
		"setexpr mmc_blocks ${filesize} + 1ff; "		\
		"setexpr mmc_blocks ${mmc_blocks} / 200\0"		\
	"update_uboot="						\
		"if test -n \"${filesize}\" -a \"${filesize}\" -gt 0; "	\
		"then "							\
			"run get_mmc_blocks; "				\
			"mmc dev 0 1; "					\
			"mmc write ${loadaddr} 0 ${mmc_blocks}; "	\
		"else "							\
			"echo Please load the U-Boot image first; "	\
		"fi\0"

#include "tegra-common-usb-gadget.h"
#include "tegra-common-post.h"

#define CONFIG_ARMV7_PSCI			1
/* Reserve top 1M for secure RAM */
#define CONFIG_ARMV7_SECURE_BASE		0xfff00000
#define CONFIG_ARMV7_SECURE_RESERVE_SIZE	0x00100000

#endif /* __CONFIG_H */
