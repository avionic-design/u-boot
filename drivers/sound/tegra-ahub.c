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

#include <common.h>
#include <asm/io.h>
#include <asm/arch/ahub.h>
#include "tegra-ahub.h"

/*
 * This driver will encapsulate the AHUB/AUDIO/APBIF/CIF parts
 * of the audio HW driver. Only 16-bit stereo audio out is supported.
 */

/*
 * Init ahub (audio/apbif) regs for I2S playback
 *
 * @param bps		bits per sample (8/16)
 * @param channels	number of channels (1/2)
 *
 * @return              int value, 0 for success
 */
int ahub_init(unsigned int bps, unsigned int channels)
{
	struct ahub_ctlr *ahub = (struct ahub_ctlr *)NV_PA_APBIF_BASE;
	struct audio_ctlr *audio = (struct audio_ctlr *)NV_PA_AUDIO_BASE;
	unsigned int bits, chan, reg, thresh;

	debug("%s: entry, bps = %d, channels = %d\n", __func__, bps,
	      channels);

	/* Set AHUB CIF info for channel 0 */
	bits = (bps >> 2) - 1;
	chan = channels - 1;
	thresh = 0;
	reg = (chan << TEGRA_AUDIOCIF_CTRL_AUDIO_CHANNELS_SHIFT) |
		(chan << TEGRA_AUDIOCIF_CTRL_CLIENT_CHANNELS_SHIFT);
	reg |= (bits << TEGRA_AUDIOCIF_CTRL_AUDIO_BITS_SHIFT) |
		(bits << TEGRA_AUDIOCIF_CTRL_CLIENT_BITS_SHIFT);

	reg |= (thresh << TEGRA_AUDIOCIF_CTRL_FIFO_THRESHOLD_SHIFT);
	writel(reg, &ahub->ahub_channel0_cif_tx0_ctrl);

	/* Set channel0 control TX_EN, threshold, etc. */
	reg = 0;
	thresh = 7;
	reg |= (thresh << TEGRA_AHUB_CHANNEL_CTRL_TX_THRESHOLD_SHIFT);
	reg |= TEGRA_AHUB_CHANNEL_CTRL_TX_EN;
	reg |= TEGRA_AHUB_CHANNEL_CTRL_TX_PACK_EN |
		TEGRA_AHUB_CHANNEL_CTRL_TX_PACK_16;
	writel(reg, &ahub->ahub_channel0_ctrl);

	/* Set up AUDIO_I2S3_RX0 connection to APBIF_TX0 (in AHUB) */
	writel(APBIF_TX0, &audio->audio_i2s3_rx0);

	return 0;
}
