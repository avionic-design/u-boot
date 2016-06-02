/*
 * Copyright (c) 2013, NVIDIA CORPORATION.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __TEGRA_AHUB_H__
#define __TEGRA_AHUB_H__

#define FIFO_LENGTH		8	/* I2S FIFOs are 16-words deep? */
#define TIMEOUT_I2S_TX		100
#define I2S1_TX_FIFO_FULL	TEGRA_AHUB_I2S_LIVE_STATUS_I2S1_TX_FIFO_FULL
#define I2S3_TX_FIFO_FULL	TEGRA_AHUB_I2S_LIVE_STATUS_I2S3_TX_FIFO_FULL
#define I2S3_TX_FIFO_EMPTY	TEGRA_AHUB_I2S_LIVE_STATUS_I2S3_TX_FIFO_EMPTY
#define APBIF_TX0		(1 << 0)

/*
 * Init ahub (audio/apbif) regs for I2S playback
 *
 * @param bps		bits per sample (8/16)
 * @param channels	number of channels (1/2)
 * @return              int value, 0 for success
 */
int ahub_init(unsigned int bps, unsigned int channels);

#endif /*__TEGRA_AHUB_H__ */
