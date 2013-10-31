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
 *
 * Portions Copyright (C) 2012 Samsung Electronics
 * R. Chandrasekar <rcsekar@samsung.com>
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/ahub.h>
#include <asm/arch/clock.h>
#include <asm/arch/pinmux.h>
#include <asm/arch/i2s.h>
#include <sound.h>
#include <i2s.h>
#include <libfdt.h>
#include <fdtdec.h>
#include "tegra-ahub.h"

/*
 * This is the Tegra30/Tegra114 I2S driver used by the 'sound' command.
 * I2S on Tegra30+ connects to the AHUB (Audio Hub) on one end, and the
 * audio codec (RealTek, Maxim, Wolfson, etc.) on the other, at least
 * as far as this code and U-Boot 'sound' is concerned. The I2S driver
 * will send an audio stream (PCM, 16-bit stereo) to the codec.
 */

/*
 * Sample rate calculations:
 * For LRCK mode, the channel_bit_cnt can be calculated as:
 *  channel_bit_cnt = (freq of bit_clk) / (2 * sample rate) - 1
 * For FSYNC mode, the channel_bit_cnt can be calculated as:
 *  channel_bit_cnt = (freq of bit_clk) / (sample rate) - 1
 */

/*
 * Sets the i2s transfer control
 *
 * @param i2s_reg	i2s register address
 * @param on		1 enable tx , 0 disable tx transfer
 */
static void i2s_txctrl(struct i2s_reg *i2s_reg, int on)
{
	debug("%s called, tx on = %d\n", __func__, on);

	/* Set I2S_CTRL[31] = 1 to enable I2S Tx mode */
	clrsetbits_le32(&i2s_reg->i2s_ctrl, (1 << 31), (on << 31));
	debug("%s: Wrote 0x%08X to I2S_CTRL reg\n", __func__,
	      readl(&i2s_reg->i2s_ctrl));
}

/*
 * Sets I2S Clock format
 *
 * @param fmt		i2s clock properties
 * @param i2s_reg	i2s register address
 *
 * @return		int value 0 for success, -1 in case of error
 */
int i2s_set_fmt(struct i2s_reg *i2s_reg, unsigned int fmt)
{
	u32 i2sctrl;

	debug("%s called, i2s clock props = 0x%04X\n", __func__, fmt);

	switch (fmt & SND_SOC_DAIFMT_INV_MASK) {
	case SND_SOC_DAIFMT_NB_NF:
		debug("%s: Format is NB_NF\n", __func__);
		break;
	default:
		return -1;
	}

	i2sctrl = readl(&i2s_reg->i2s_ctrl);
	i2sctrl &= ~I2S_CTRL_MASTER_ENABLE;

	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	case SND_SOC_DAIFMT_CBS_CFS:
		debug("%s: Format is CBS_CFS\n", __func__);
		i2sctrl |= I2S_CTRL_MASTER_ENABLE;
		break;
	case SND_SOC_DAIFMT_CBM_CFM:
		debug("%s: Format is CBM_CFM\n", __func__);
		break;
	default:
		return -1;
	}

	i2sctrl &= ~(I2S_CTRL_FRAME_FORMAT_MASK | I2S_CTRL_LRCK_MASK);
	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case SND_SOC_DAIFMT_DSP_A:
		debug("%s: Format is DSP_A\n", __func__);
		i2sctrl |= I2S_CTRL_FRAME_FORMAT_FSYNC;
		i2sctrl |= I2S_CTRL_LRCK_L_LOW;
		break;
	case SND_SOC_DAIFMT_DSP_B:
		debug("%s: Format is DSP_B\n", __func__);
		i2sctrl |= I2S_CTRL_FRAME_FORMAT_FSYNC;
		i2sctrl |= I2S_CTRL_LRCK_R_LOW;
		break;
	case SND_SOC_DAIFMT_I2S:
		debug("%s: Format is I2S\n", __func__);
		i2sctrl |= I2S_CTRL_FRAME_FORMAT_LRCK;
		i2sctrl |= I2S_CTRL_LRCK_L_LOW;
		break;
	case SND_SOC_DAIFMT_RIGHT_J:
		debug("%s: Format is RJ\n", __func__);
		i2sctrl |= I2S_CTRL_FRAME_FORMAT_LRCK;
		i2sctrl |= I2S_CTRL_LRCK_L_LOW;
		break;
	case SND_SOC_DAIFMT_LEFT_J:
		debug("%s: Format is LJ\n", __func__);
		i2sctrl |= I2S_CTRL_FRAME_FORMAT_LRCK;
		i2sctrl |= I2S_CTRL_LRCK_L_LOW;
		break;
	default:
		return -1;
	}
	writel(i2sctrl, &i2s_reg->i2s_ctrl);
	debug("%s: Wrote 0x%08X to I2S_CTRL\n", __func__, i2sctrl);

	return 0;
}

/*
 * Instead of using DMA, transfer sq wave data to TXFIFO w/FIFO full check.
 * TODO(twarren@nvidia.com): Use APBDMA
 */

int i2s_transfer_tx_data(struct i2stx_info *pi2s_tx, unsigned int *data,
			 unsigned long data_size)
{
	int i, start;
	struct i2s_reg *i2s_reg = (struct i2s_reg *)pi2s_tx->base_address;
	struct ahub_ctlr *ahub = (struct ahub_ctlr *)NV_PA_APBIF_BASE;

	debug("%s called, data size = %lu\n", __func__, data_size);

	if (data_size < FIFO_LENGTH) {
		debug("%s : Invalid data size\n", __func__);
		return -1; /* invalid pcm data size */
	}

	ahub_init(pi2s_tx->bitspersample, pi2s_tx->channels);

	/* Fill the TX buffer before starting the TX transmit */
	for (i = 0; i < FIFO_LENGTH; i++)
		writel(*data++, &ahub->ahub_channel0_txfifo);

	data_size -= FIFO_LENGTH;
	i2s_txctrl(i2s_reg, I2S_TX_ON);

	while (data_size > 0) {
		start = get_timer(0);

		if (!(I2S1_TX_FIFO_FULL & readl(&ahub->ahub_i2s_live_stat))) {
			writel(*data++, &ahub->ahub_channel0_txfifo);
			data_size--;
		} else {
			debug("%s: FIFO full: data_size %lu\n", __func__,
			      data_size);
			if (get_timer(start) > TIMEOUT_I2S_TX) {
				i2s_txctrl(i2s_reg, I2S_TX_OFF);
				debug("%s: I2S Transfer Timeout\n", __func__);
				return -1;
			}
		}
	}
	i2s_txctrl(i2s_reg, I2S_TX_OFF);

	return 0;
}

int i2s_tx_init(struct i2stx_info *pi2s_tx)
{
	int ret;
	struct i2s_reg *i2s_reg = (struct i2s_reg *)pi2s_tx->base_address;
	unsigned int i2s_rate, chan_bit_cnt, reg, audio_bits, tmp;

	debug("%s called, base addr = 0x%08X\n",
	      __func__, pi2s_tx->base_address);

	/*
	 * Setup I2S clock, 4.8MHz using PLLP.
	 * NOTE: clock_xxx functions for Tegra are not fully
	 * cognizant of PLLA and it's ref clock of 9.6MHz,
	 * so we can't use them for this (yet). We use PLLP
	 * instead.
	 */

	reset_periph(pi2s_tx->periph_id, 2);
	i2s_rate = clock_start_periph_pll(pi2s_tx->periph_id, CLOCK_ID_PERIPH,
					  pi2s_tx->audio_pll_clk);

	debug("%s: I2S clock rate s/b= %d\n", __func__, i2s_rate);
	debug("%s: I2S clk src reg = 0x%08X\n", __func__, readl(0x60006100));
	debug("%s: PLLP = %d\n", __func__, clock_get_rate(CLOCK_ID_PERIPH));
	debug("%s: PLLA = %d\n", __func__, clock_get_rate(CLOCK_ID_AUDIO));
	debug("%s: OSC = %d\n", __func__, clock_get_rate(CLOCK_ID_OSC));

	ret = i2s_set_fmt(i2s_reg, (SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_NB_NF |
		SND_SOC_DAIFMT_CBS_CFS));
	if (ret == 0) {
		/* disable i2s transfer flag */
		i2s_txctrl(i2s_reg, I2S_TX_OFF);
	} else {
		debug("%s: failed\n", __func__);
	}

	/* I2S_CTRL (except XFER_EN) */
	audio_bits = (pi2s_tx->bitspersample >> 2) - 1;
	reg = audio_bits | (1 << 10);		/* MASTER */
	writel(reg, &i2s_reg->i2s_ctrl);

	/* I2S_TIMING */
	chan_bit_cnt = (pi2s_tx->audio_pll_clk / (pi2s_tx->samplingrate*2)) - 1;
	writel(chan_bit_cnt, &i2s_reg->i2s_timing);

	/* I2S_OFFSET */
	reg = 0x00010001;	/* RX/TX_DATA_OFFSET = 1 bit clock */
	writel(reg, &i2s_reg->i2s_offset);

	/* I2S_CH_CTRL */
	reg = (1 << 24);	/* FSYNC_WIDTH = 2 clocks wide */
	writel(reg, &i2s_reg->i2s_ch_ctrl);

	/* I2S_SLOT_CTRL */
	reg = 1;		/* TOTAL_SLOTS = 2 slots per fsync */
	writel(reg, &i2s_reg->i2s_slot_ctrl);

	/* I2S_AUDIOCIF_I2STX_CTRL */
	tmp = pi2s_tx->channels - 1;
	reg = (tmp << TEGRA_AUDIOCIF_CTRL_AUDIO_CHANNELS_SHIFT) |
		(tmp << TEGRA_AUDIOCIF_CTRL_CLIENT_CHANNELS_SHIFT);
	reg |= (audio_bits << TEGRA_AUDIOCIF_CTRL_AUDIO_BITS_SHIFT) |
		(audio_bits << TEGRA_AUDIOCIF_CTRL_CLIENT_BITS_SHIFT);
	reg |= (4 << TEGRA_AUDIOCIF_CTRL_FIFO_THRESHOLD_SHIFT);	/* THRESHOLD */
	writel(reg, &i2s_reg->i2s_cif_tx_ctrl);
	reg |= (1 << 2);		/* DIRECTION = RXCIF */
	writel(reg, &i2s_reg->i2s_cif_tx_ctrl);

	return 0;
}

/*
 * get_sound_i2s_values gets values for I2S parameters from DT
 *
 * @param i2stx_info	I2S transmitter transfer param structure
 * @param blob		FDT blob if enabled else NULL
 */
int get_sound_i2s_values(struct i2stx_info *i2s, const void *blob)
{
	int node, error = 0;
	unsigned int base;

	node = fdtdec_next_compatible(blob, 0,
					COMPAT_NVIDIA_TEGRA114_SOUND);
	if (node <= 0) {
		debug("Tegra, sound: No node for sound in device tree\n");
		return -1;
	}

	/*
	 * Get the pre-defined sound specific values from FDT.
	 * All of these are expected to be correct otherwise
	 * wrong register values in I2S setup parameters may
	 * result in no sound play.
	 */
	base = fdtdec_get_addr(blob, node, "reg");
	if (base == FDT_ADDR_T_NONE) {
		debug("%s: Missing I2S reg base address\n", __func__);
		return -1;
	}
	i2s->base_address = base;

	i2s->audio_pll_clk = fdtdec_get_int(blob, node,
			"nvidia,i2s-pll-clock-frequency", -1);
	error |= i2s->audio_pll_clk;
	i2s->samplingrate = fdtdec_get_int(blob, node,
			"nvidia,i2s-sampling-rate", -1);
	error |= i2s->samplingrate;
	i2s->bitspersample = fdtdec_get_int(blob, node,
			"nvidia,i2s-bits-per-sample", -1);
	error |= i2s->bitspersample;
	i2s->channels = fdtdec_get_int(blob, node,
			"nvidia,i2s-channels", -1);
	error |= i2s->channels;
	i2s->rfs = fdtdec_get_int(blob, node,
			"nvidia,i2s-lr-clk-framesize", -1);
	error |= i2s->rfs;
	i2s->bfs = fdtdec_get_int(blob, node,
			"nvidia,i2s-bit-clk-framesize", -1);
	error |= i2s->bfs;
	i2s->periph_id = clock_decode_periph_id(blob, node);

	if (error == -1) {
		debug("%s: failed to get I2S node properties\n", __func__);
		return -1;
	}

	debug("I2S base addr  = 0x%08X\n", base);
	debug("periph_id = %d\n", i2s->periph_id);
	debug("audio_pll_clk = %d\n", i2s->audio_pll_clk);
	debug("samplingrate = %d\n", i2s->samplingrate);
	debug("bitspersample = %d\n", i2s->bitspersample);
	debug("channels = %d\n", i2s->channels);
	debug("rfs = %d\n", i2s->rfs);
	debug("bfs = %d\n", i2s->bfs);

	return 0;
}
