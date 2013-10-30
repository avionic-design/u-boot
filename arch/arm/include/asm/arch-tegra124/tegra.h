/*
 * (C) Copyright 2013
 * NVIDIA Corporation <www.nvidia.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#ifndef _TEGRA124_H_
#define _TEGRA124_H_

#define NV_PA_SDRAM_BASE	0x80000000
#define NV_PA_TSC_BASE		0x700F0000	/* System Counter TSC regs */
#define NV_PA_MC_BASE		0x70019000	/* Mem Ctlr regs (MCB, etc.) */
#define NV_PA_AHB_BASE		0x6000C000	/* System regs (AHB, etc.) */
/* Place-holders for now, until these are fully in device-tree */
#define NV_PA_AHUB_BASE		0x70300000	/* Audio Hub, 8KB total */
#define NV_PA_APBIF_BASE	0x70300000	/* APBIF block regs */
#define NV_PA_APBIF2_BASE	0x70300200	/* APBIF2 block regs */
#define NV_PA_AUDIO_BASE	0x70300800	/* Audio block regs */
#define NV_PA_I2S0_BASE		0x70301000	/* I2S0 block regs */
#define NV_PA_I2S1_BASE		0x70301100	/* I2S1 block regs */
#define NV_PA_I2S2_BASE		0x70301200	/* I2S2 block regs */
#define NV_PA_I2S3_BASE		0x70301300	/* I2S3 block regs */
#define NV_PA_I2S4_BASE		0x70301400	/* I2S4 block regs */
#define NV_PA_DAM0_BASE		0x70302000	/* DAM0 block regs */
#define NV_PA_DAM1_BASE		0x70302200	/* DAM1 block regs */
#define NV_PA_DAM2_BASE		0x70302400	/* DAM2 block regs */
/* SPDIF/AMXn/ADXn not used */
#define NV_PA_TSC_BASE		0x700F0000	/* System Counter TSC regs */
#define NV_PA_SDRAM_BASE	0x80000000	/* 0x80000000 for real T1x4 */

#include <asm/arch-tegra/tegra.h>

#define BCT_ODMDATA_OFFSET	1704	/* offset to ODMDATA word */

#undef NVBOOTINFOTABLE_BCTSIZE
#undef NVBOOTINFOTABLE_BCTPTR
#define NVBOOTINFOTABLE_BCTSIZE	0x48	/* BCT size in BIT in IRAM */
#define NVBOOTINFOTABLE_BCTPTR	0x4C	/* BCT pointer in BIT in IRAM */

#define MAX_NUM_CPU		4
#define MCB_EMEM_ARB_OVERRIDE	(NV_PA_MC_BASE + 0xE8)

#define TEGRA_USB1_BASE		0x7D000000

#endif /* _TEGRA124_H_ */
