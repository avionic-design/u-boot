/*
 * Copyright (c) 2013, NVIDIA CORPORATION.  All rights reserved.
 *
 * Portions Copyright (C) 2012 Samsung Electronics
 * R. Chandrasekar <rcsekar@samsung.com>
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

#include <malloc.h>
#include <common.h>
#include <asm/io.h>
#include <libfdt.h>
#include <fdtdec.h>
#include <asm/arch/clock.h>
#include <i2c.h>
#include <i2s.h>
#include <sound.h>
#include <asm/arch/sound.h>
#include <asm/arch/ahub.h>
#if defined(CONFIG_SOUND_RT5640)
#include "rt5640.h"
#elif defined(CONFIG_SOUND_MAX98090)
#include "max98090.h"
#include "maxim_codec.h"
#endif

/* defines */
#define SOUND_400_HZ		400
#define SOUND_BITS_IN_BYTE	8

/*
 * This is the 'sound' driver for Tegra114. Audio on Tegra30/114
 * consists of the Audio hub (AHUB), AP Bus I/F (APBIF), and Client I/F
 * (CIF) devices, plus the I2S/SPDIF serial in/out devices to get
 * audio in and out of a codec. There are also DAM, AMX and DMX
 * devices (Digital Audio Mixer, Audio Multiplexer, and
 * DeMultiplexer) that aren't used here.
 *
 * The I2S driver is in tegra_i2s.c, and the audio codec driver(s)
 * are named for the part (RT5640 is the RealTek ALC5640 driver,
 * etc.). The codec driver uses modified I2C access functions
 * (multibyte) in tegra_i2c.c to read/write the 16-bit codec regs.
 *
 * This driver will encapsulate the AHUB/AUDIO/APBIF/CIF parts
 * of the audio HW driver. Only 16-bit stereo audio out is supported.
 */

static struct i2stx_info g_i2stx_pri;

/*
 * Init codec
 *
 * @param blob          FDT blob
 * @param pi2s_tx	i2s parameters required by codec
 * @return              int value, 0 for success
 */
static int codec_init(const void *blob, struct i2stx_info *pi2s_tx)
{
	int ret;
	const char *codectype;
	int node;

	/* Get the node from FDT for sound */
	node = fdtdec_next_compatible(blob, 0, COMPAT_NVIDIA_TEGRA114_SOUND);
	if (node <= 0) {
		debug("Tegra, sound: No node for sound in device tree\n");
		debug("node = %d\n", node);
		return -1;
	}

	/*
	 * Get the pre-defined sound codec specific values from FDT.
	 * All of these are expected to be correct otherwise sound
	 * can not be played
	 */
	codectype = fdt_getprop(blob, node, "nvidia,codec-type", NULL);
	debug("device = %s\n", codectype);

#if defined(CONFIG_SOUND_RT5640)
	if (!strcmp(codectype, "rt5640")) {
		/* Check the codec type and initialise the same */
		ret = rt5640_init(blob, RT5640_AIF1, pi2s_tx->samplingrate,
			(pi2s_tx->samplingrate * (pi2s_tx->rfs)),
			pi2s_tx->bitspersample, pi2s_tx->channels);
	}
#elif defined(CONFIG_SOUND_MAX98090)
	if (!strcmp(codectype, "max98090")) {
		ret = maxim_codec_init(blob, codectype, pi2s_tx->samplingrate,
			(pi2s_tx->samplingrate * (pi2s_tx->rfs)),
			pi2s_tx->bitspersample);
	}
#else
	if (0) {
	}
#endif
	else {
		debug("%s: Unknown codec type %s\n", __func__, codectype);
		ret = 0;
	}

	if (ret) {
		debug("%s: Codec init failed\n", __func__);
		return -1;
	}

	return 0;
}

void set_mclk_from_ap(void)
{
	u32 reg;
	debug("%s: Set codec MCLK to CLK1_OUT, 12MHz OSC\n", __func__);

	reg = readl(0x60006360);	/* CLK_ENB_V */
	reg |= (1 << 26);		/* CLK_ENB_EXTPERIPH3 */
	writel(reg, 0x60006360);
	reg = readl(0x7000E5A8);	/* PMC_CLK_CNTRL */
	reg |= (1 << 18);		/* CLK3_EN */
	writel(reg, 0x7000E5A8);
	/* CLK1_OUT -> codec MCLK s/b running at 12MHz (OSC) now */
	writel(0x60000000, 0x600063EC);
}

int sound_init(const void *blob)
{
	int ret;
	struct i2stx_info *pi2s_tx = &g_i2stx_pri;

	/* Get the I2S Values */
	if (get_sound_i2s_values(pi2s_tx, blob) < 0) {
		debug(" FDT I2S values failed\n");
		return -1;
	}

	/* Set up the codec MCLK from Tegra */
	set_mclk_from_ap();

	if (codec_init(blob, pi2s_tx) < 0) {
		debug(" Codec init failed\n");
		return -1;
	}

	ret = i2s_tx_init(pi2s_tx);
	if (ret) {
		debug("%s: Failed to init i2c transmit: ret=%d\n", __func__,
		      ret);
		return ret;
	}


	return ret;
}

/*
 * Generates square wave sound data for 1 second
 *
 * @param data          data buffer pointer
 * @param size          size of the buffer
 * @param freq          frequency of the wave
 */
static void sound_prepare_buffer(unsigned short *data, int size, uint32_t freq)
{
	const int sample = 48000;
	const unsigned short amplitude = 16000; /* between 1 and 32767 */
	const int period = freq ? sample / freq : 0;
	const int half = period / 2;

	assert(freq);

	/* Make sure we don't overflow our buffer */
	if (size % 2)
		size--;

	while (size) {
		int i;
		for (i = 0; size && i < half; i++) {
			size -= 2;
			*data++ = amplitude;
			*data++ = amplitude;
		}
		for (i = 0; size && i < period - half; i++) {
			size -= 2;
			*data++ = -amplitude;
			*data++ = -amplitude;
		}
	}
}

int sound_play(uint32_t msec, uint32_t frequency)
{
	unsigned int *data;
	unsigned long data_size;
	unsigned int ret = 0;

	/* Buffer length computation */
	data_size = g_i2stx_pri.samplingrate * g_i2stx_pri.channels;
	data_size *= (g_i2stx_pri.bitspersample / SOUND_BITS_IN_BYTE);
	data = malloc(data_size);
	debug("%s: data_size = %ld, data @ 0x%08X\n", __func__, data_size,
	      (unsigned)data);

	if (data == NULL) {
		debug("%s: malloc failed\n", __func__);
		return -1;
	}

	sound_prepare_buffer((unsigned short *)data,
			     data_size / sizeof(unsigned short), frequency);

	while (msec >= 1000) {
		ret = i2s_transfer_tx_data(&g_i2stx_pri, data,
					   (data_size / sizeof(int)));
		msec -= 1000;
	}
	if (msec) {
		unsigned long size =
			(data_size * msec) / 1000;

		ret = i2s_transfer_tx_data(&g_i2stx_pri, data, size);
	}

	debug("%s: I2S register dump:\n", __func__);

	free(data);

	return ret;
}
