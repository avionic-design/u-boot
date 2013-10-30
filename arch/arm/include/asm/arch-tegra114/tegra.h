/*
 * Copyright (c) 2010-2013, NVIDIA CORPORATION.  All rights reserved.
 *
 * SPDX-License-Identifier:	GPL-2.0
 */

#ifndef _TEGRA114_H_
#define _TEGRA114_H_

#define NV_PA_SDRAM_BASE	0x80000000	/* 0x80000000 for real T114 */
#define NV_PA_TSC_BASE		0x700F0000	/* System Counter TSC regs */
#define NV_PA_MC_BASE		0x70019000
/* Place-holders for now, until these are fully in device-tree */
#define NV_PA_AHUB_BASE		0x70080000	/* Audio Hub, 8KB total */
#define NV_PA_APBIF_BASE	0x70080000	/* APBIF block regs*/
#define NV_PA_AUDIO_BASE	0x70080200	/* Audio block regs */
#define NV_PA_I2S0_BASE		0x70080300	/* I2S0 block regs */
#define NV_PA_I2S1_BASE		0x70080400	/* I2S1 block regs */
#define NV_PA_I2S2_BASE		0x70080500	/* I2S2 block regs */
#define NV_PA_I2S3_BASE		0x70080600	/* I2S3 block regs */
#define NV_PA_I2S4_BASE		0x70080700	/* I2S4 block regs */
#define NV_PA_DAM0_BASE		0x70080800	/* DAM0 block regs */
#define NV_PA_DAM1_BASE		0x70080900	/* DAM1 block regs */
#define NV_PA_DAM2_BASE		0x70080A00	/* DAM2 block regs */
/* SPDIF/AMX/ADX not used */
#define NV_PA_TSC_BASE		0x700F0000	/* System Counter TSC regs */
#define NV_PA_SDRAM_BASE	0x80000000	/* 0x80000000 for real T114 */

#include <asm/arch-tegra/tegra.h>

#define BCT_ODMDATA_OFFSET	1752	/* offset to ODMDATA word */

#undef NVBOOTINFOTABLE_BCTSIZE
#undef NVBOOTINFOTABLE_BCTPTR
#define NVBOOTINFOTABLE_BCTSIZE        0x48    /* BCT size in BIT in IRAM */
#define NVBOOTINFOTABLE_BCTPTR 0x4C    /* BCT pointer in BIT in IRAM */

#define MAX_NUM_CPU            4

#endif /* TEGRA114_H */
