/*
 * tlv320dac3100.c -- Codec driver for TI TLV320DAC3100
 *
 * Copyright (c) 2015 Avionic Design GmbH
 *
 * Authors: Alban Bedel <alban.bedel@avionic-design.de>
 *          Julian Scheel <julian@jusst.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/log2.h>
#include <common.h>
#include <asm/gpio.h>
#include <asm/arch/gpio.h>
#include <sound.h>
#include <div64.h>
#include <i2c.h>
#include "i2s.h"

#define DAC3100_PAGE_SIZE 128
#define DAC3100_MAX_PAGES 13
#define DAC3100_MAX_REGISTERS (DAC3100_MAX_PAGES * DAC3100_PAGE_SIZE)

#define DAC3100_REG(p, r) ((p) * DAC3100_PAGE_SIZE + (r))

#define DAC3100_REG_PAGE(r) ((r) / DAC3100_PAGE_SIZE)
#define DAC3100_REG_ADDR(r) ((r) % DAC3100_PAGE_SIZE)

#define DAC3100_RESET			DAC3100_REG(0, 1)

#define DAC3100_CLOCK_GEN_MUX		DAC3100_REG(0, 4)
#define DAC3100_PLL_P_R			DAC3100_REG(0, 5)
#define DAC3100_PLL_J			DAC3100_REG(0, 6)
#define DAC3100_PLL_D_MSB		DAC3100_REG(0, 7)
#define DAC3100_PLL_D_LSB		DAC3100_REG(0, 8)

#define DAC3100_DAC_NDAC		DAC3100_REG(0, 11)
#define DAC3100_DAC_MDAC		DAC3100_REG(0, 12)
#define DAC3100_DAC_DOSR_MSB		DAC3100_REG(0, 13)
#define DAC3100_DAC_DOSR_LSB		DAC3100_REG(0, 14)

#define DAC3100_CODEC_IFACE_CTRL1	DAC3100_REG(0, 27)
#define DAC3100_DATA_SLOT_OFFSET	DAC3100_REG(0, 28)
#define DAC3100_CODEC_IFACE_CTRL2	DAC3100_REG(0, 29)

#define DAC3100_DAC_FLAGS_0		DAC3100_REG(0, 37)
#define DAC3100_DAC_FLAGS_1		DAC3100_REG(0, 38)
#define DAC3100_OVERFLOW_FLAGS		DAC3100_REG(0, 39)
#define DAC3100_DAC_INT_FLAGS		DAC3100_REG(0, 44)
#define DAC3100_DAC_INT_STATUS		DAC3100_REG(0, 46)
#define DAC3100_GPIO1_CTRL		DAC3100_REG(0, 51)
#define DAC3100_DIN_CTRL		DAC3100_REG(0, 54)

#define DAC3100_DAC_PROCESSING_BLOCK	DAC3100_REG(0, 60)

#define DAC3100_DAC_DATA_PATH_SETUP	DAC3100_REG(0, 63)
#define DAC3100_DAC_VOLUME		DAC3100_REG(0, 64)
#define DAC3100_DAC_LEFT_VOLUME		DAC3100_REG(0, 65)
#define DAC3100_DAC_RIGHT_VOLUME	DAC3100_REG(0, 66)
#define DAC3100_HEADSET_DETECT		DAC3100_REG(0, 67)

#define DAC3100_LEFT_BEEP_GEN		DAC3100_REG(0, 71)
#define DAC3100_RIGHT_BEEP_GEN		DAC3100_REG(0, 72)

#define DAC3100_MICDET_GAIN		DAC3100_REG(0, 117)

#define DAC3100_HP_DRIVER		DAC3100_REG(1, 31)
#define DAC3100_SPK_AMP			DAC3100_REG(1, 32)

#define DAC3100_DAC_MIXER		DAC3100_REG(1, 35)
#define DAC3100_LEFT_VOL_HPL		DAC3100_REG(1, 36)
#define DAC3100_RIGHT_VOL_HPR		DAC3100_REG(1, 37)
#define DAC3100_LEFT_VOL_SPK		DAC3100_REG(1, 38)
#define DAC3100_HPL_DRIVER		DAC3100_REG(1, 40)
#define DAC3100_HPR_DRIVER		DAC3100_REG(1, 41)
#define DAC3100_SPK_DRIVER		DAC3100_REG(1, 42)

#define DAC3100_MICBIAS			DAC3100_REG(1, 46)

#define DAC3100_DAC_COEF_RAM		DAC3100_REG(8, 1)

#define DAC3100_PLL_CLK_MIN	80000000
#define DAC3100_PLL_CLK_MAX	110000000

#define DAC3100_DAC_MOD_CLK_MIN	2800000
#define DAC3100_DAC_MOD_CLK_MAX	6200000

struct dac3100 {
	struct udevice *i2c;
	struct sound_codec_info codec;
	int reset_gpio;
	int amp_gpio;

	u8 page;
	unsigned clkin_rate;
	unsigned clkin_src;
};

struct dac3100 g_dac3100;

static bool dac3100_reg_page_is_valid(unsigned int reg)
{
	switch (DAC3100_REG_PAGE(reg)) {
	case 0:
	case 1:
	case 3:
	case 8:
	case 9:
	case 12:
	case 13:
		return true;
	default:
		return false;
	}
}

static bool dac3100_readable_register(unsigned int reg)
{
	if (!dac3100_reg_page_is_valid(reg))
		return false;
	return true;
}

static bool dac3100_writable_register(unsigned int reg)
{
	if (!dac3100_reg_page_is_valid(reg))
		return false;
	return true;
}

static bool dac3100_volatile_register(unsigned int reg)
{
	switch (reg) {
	case DAC3100_RESET:
	case DAC3100_DAC_FLAGS_0:
	case DAC3100_DAC_FLAGS_1:
	case DAC3100_OVERFLOW_FLAGS:
	case DAC3100_DAC_INT_FLAGS:
	case DAC3100_DAC_INT_STATUS:
	case DAC3100_GPIO1_CTRL:
	case DAC3100_DIN_CTRL:
	case DAC3100_HEADSET_DETECT:
	case DAC3100_LEFT_BEEP_GEN:
	case DAC3100_MICDET_GAIN:
	case DAC3100_HP_DRIVER:
	case DAC3100_SPK_AMP:
	case DAC3100_HPL_DRIVER:
	case DAC3100_HPR_DRIVER:
	case DAC3100_SPK_DRIVER:
	case DAC3100_DAC_COEF_RAM:
		return true;
	default:
		return false;
	}
}

static bool dac3100_precious_register(unsigned int reg)
{
	switch (reg) {
	case DAC3100_OVERFLOW_FLAGS:
	case DAC3100_DAC_INT_FLAGS:
		return true;
	default:
		return false;
	}
}

static int dac3100_i2c_update_page(struct dac3100 *dac, unsigned int page)
{
	if (dac->page != page) {
		dac->page = page;
		dm_i2c_write(dac->i2c, 0x00, &dac->page, 1);
	}

	return 0;
}

static int dac3100_i2c_write(struct dac3100 *dac, unsigned int reg,
		unsigned char data)
{
	if (!dac3100_writable_register(reg)) {
		error("%s: Register 0x%02x is not writable\n", __func__,
				reg);
		return -1;
	}
	dac3100_i2c_update_page(dac, DAC3100_REG_PAGE(reg));

	reg = DAC3100_REG_ADDR(reg);
	debug("%s: Write addr: 0x%02x, data: 0x%02x\n", __func__, reg, data);
	return dm_i2c_write(dac->i2c, reg, &data, 1);
}

static u8 dac3100_i2c_read(struct dac3100 *dac, unsigned int reg)
{
	int ret;
	u8 data;

	if (!dac3100_readable_register(reg)) {
		error("%s: Register 0x%02x is not readable\n", __func__,
				reg);
		return 0;
	}
	dac3100_i2c_update_page(dac, DAC3100_REG_PAGE(reg));

	reg = DAC3100_REG_ADDR(reg);
	ret = dm_i2c_read(dac->i2c, reg, &data, 1);
	if (ret != 0) {
		debug("%s(): Error while reading register 0x%02x\n", __func__,
				reg);
		return 0;
	}

	return data;
}

static int dac3100_set_dai_sysclk(struct dac3100 *dac,
				int clk_id, unsigned int freq, int dir)
{
	dac->clkin_src = clk_id;
	dac->clkin_rate = freq;

	return 0;
}

static int dac3100_hw_params(struct dac3100 *dac,
		unsigned channels, unsigned fs, unsigned bits_per_sample)
{
	int dosr, dosr_round;
	int mdiv, ndac, mdac;
	int filter, pb, rc;
	int jd = 10000;
	int word_len;
	int clkmux;
	int clkin;
	u8 ctrl1;
	int err;

	/* Check the word length */
	switch (bits_per_sample) {
	case 16:
		word_len = 0;
		break;
	case 20:
		word_len = 1;
		break;
	case 24:
		word_len = 2;
		break;
	case 32:
		word_len = 3;
		break;
	default:
		return -EINVAL;
	}

	/* Select the filter and DOSR rounding according to the samplerate */
	if (fs > 96000) {
		filter = 2;
		dosr_round = 2;
	} else if (fs > 48000) {
		filter = 1;
		dosr_round = 4;
	} else {
		filter = 0;
		dosr_round = 8;
	}

	/* TODO: When implementing filters replace this with a table lookup
	 * to choose the best processing block. */
	switch(filter) {
	case 0:
		if (channels > 1) {
			pb = 1;
			rc = 8;
		} else {
			pb = 4;
			rc = 4;
		}
		break;
	case 1:
		if (channels > 1) {
			pb = 7;
			rc = 6;
		} else {
			pb = 12;
			rc = 3;
		}
		break;
	case 2:
		if (channels > 1) {
			pb = 17;
			rc = 3;
		} else {
			pb = 20;
			rc = 2;
		}
		break;
	default:
		return -EINVAL;
	}

	/* If the mclk is not a multiple of the samplerate
	 * we need to use the fractional PLL to produce such a rate */
	if (dac->clkin_rate % (fs * channels * bits_per_sample)) {
		/* The PLL output must be between 80 and 110MHz */
		int mult = roundup_pow_of_two(DAC3100_PLL_CLK_MIN / fs);
		while (mult <= 1024 * 128 * 128 &&
				fs * mult <= DAC3100_PLL_CLK_MAX) {
			u64 c = (u64)fs * mult * 10000;
			if (!do_div(c, dac->clkin_rate) && c >= 10000) {
				jd = c;
				break;
			}
			mult *= 2;
		}

		if (mult > 1024 * 128 * 128 ||
				fs * mult > DAC3100_PLL_CLK_MAX) {
			error("Couldn't setup fractional divider\n");
			return -EINVAL;
		}
		clkin = fs * mult;
	} else {
		clkin = dac->clkin_rate;
		/* Check that the clock is fast enough, if not add a multiplier */
		if (clkin < rc * fs * 32) {
			int mult = DIV_ROUND_UP(80000000, clkin);

			/* Check that we are still in the range of the PLL */
			if (clkin * mult > 110000000 || mult < 4 || mult > 63) {
				error("Couldn't find multiplier\n");
				return -EINVAL;
			}

			jd = mult * 10000;
			clkin *= mult;
		}
	}

	/* Find the highest possible DOSR value */
	dosr = DAC3100_DAC_MOD_CLK_MAX / fs;
	dosr = dosr / dosr_round * dosr_round;

	/* Look for a DOSR value that is a multiple of FS
	 * and need an acceptable divider */
	while (dosr * fs >= DAC3100_DAC_MOD_CLK_MIN) {
		mdiv = clkin / (dosr * fs);
		if (mdiv * dosr * fs == clkin && mdiv < 128 * 128)
			break;
		dosr -= dosr_round;
	}

	if (dosr * fs < DAC3100_DAC_MOD_CLK_MIN) {
		error("Failed to find clock setup\n");
		return -EINVAL;
	}

	/* Get the smallest possible MDAC with a valid NDAC */
	for (mdac = max(rc * 32 / dosr, 1); mdac <= 128; mdac++) {
		if (mdiv % mdac == 0 && mdiv / mdac <= 128)
			break;
	}

	if (mdac > 128) {
		error("Failed to find divider setup\n");
		return -EINVAL;
	}

	ndac = mdiv / mdac;

	if (clkin / ndac > 48000000) {
		error("Failed to find divider setup\n");
		return -EINVAL;
	}

	debug("codec settings: sysclk=%d, clkin=%d, "
		"jd=%d, ndac=%d, mdac=%d, dosr=%d, pb=%d, rc=%d\n",
		dac->clkin_rate, clkin, jd, ndac, mdac, dosr, pb, rc);

	/* Make sure the dividers and PLL are stopped */
	err = dac3100_i2c_write(dac, DAC3100_DAC_MDAC, 0);
	if (err)
		goto error;
	err = dac3100_i2c_write(dac, DAC3100_DAC_NDAC, 0);
	if (err)
		goto error;
	err = dac3100_i2c_write(dac, DAC3100_PLL_P_R, 0x11);
	if (err)
		goto error;

	/* Setup the clock mux */
	clkmux = dac->clkin_src & 3;
	if (jd > 10000)
		clkmux = (clkmux << 2) | 3;

	err = dac3100_i2c_write(dac, DAC3100_CLOCK_GEN_MUX, clkmux);
	if (err)
		goto error;

	/* Setup the PLL if needed */
	if (jd > 10000) {
		err = dac3100_i2c_write(dac, DAC3100_PLL_J, jd / 10000);
		if (err)
			goto error;

		err = dac3100_i2c_write(dac, DAC3100_PLL_D_MSB,
				(jd % 10000) >> 8);
		if (err)
			goto error;
		err = dac3100_i2c_write(dac, DAC3100_PLL_D_LSB,
				(jd % 10000) & 0xFF);
		if (err)
			goto error;

		/* Start the PLL and wait for the lock */
		err = dac3100_i2c_write(dac, DAC3100_PLL_P_R, 0x91);
		if (err)
			goto error;
		udelay(10 * 1000);
	}

	/* Configure the dividers */
	err = dac3100_i2c_write(dac, DAC3100_DAC_NDAC, (ndac & 0x7f) | BIT(7));
	if (err)
		goto error_stop_pll;
	err = dac3100_i2c_write(dac, DAC3100_DAC_MDAC, (mdac & 0x7f) | BIT(7));
	if (err)
		goto error_ndac;
	err = dac3100_i2c_write(dac, DAC3100_DAC_DOSR_MSB, (dosr >> 8) & 3);
	if (err)
		goto error_mdac;
	err = dac3100_i2c_write(dac, DAC3100_DAC_DOSR_LSB, dosr & 0xff);
	if (err)
		goto error_mdac;

	/* Setup the word size */
	ctrl1 = dac3100_i2c_read(dac, DAC3100_CODEC_IFACE_CTRL1);
	ctrl1 &= ~(3 << 4);
	ctrl1 |= (word_len << 4);
	err = dac3100_i2c_write(dac, DAC3100_CODEC_IFACE_CTRL1, ctrl1);
	if (err < 0)
		goto error_mdac;

	/* Setup the processing block */
	err = dac3100_i2c_write(dac, DAC3100_DAC_PROCESSING_BLOCK, pb);
	if (err)
		goto error_mdac;

	return 0;

error_mdac:
	dac3100_i2c_write(dac, DAC3100_DAC_MDAC, 0);
error_ndac:
	dac3100_i2c_write(dac, DAC3100_DAC_NDAC, 0);
error_stop_pll:
	dac3100_i2c_write(dac, DAC3100_PLL_P_R, 0x11);
error:
	return err;
}

static int dac3100_set_dai_fmt(struct dac3100 *dac, unsigned int fmt)
{
	u8 ctrl1 = dac3100_i2c_read(dac, DAC3100_CODEC_IFACE_CTRL1);
	u8 ctrl2 = dac3100_i2c_read(dac, DAC3100_CODEC_IFACE_CTRL2);
	int err;

	/* Clear everything except the bit per samples */
	ctrl1 &= ~(3 << 4);

	/* Set the clocks direction */
	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	case SND_SOC_DAIFMT_CBS_CFS:
		break;
	case SND_SOC_DAIFMT_CBS_CFM:
		ctrl1 |= 1 << 2;
		break;
	case SND_SOC_DAIFMT_CBM_CFS:
		ctrl1 |= 2 << 2;
		break;
	case SND_SOC_DAIFMT_CBM_CFM:
		ctrl1 |= 3 << 2;
		break;
	default:
		return -EINVAL;
	}

	/* Set the data format */
	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case SND_SOC_DAIFMT_I2S:
		break;
	case SND_SOC_DAIFMT_DSP_A:
		ctrl1 |= 1 << 6;
		break;
	case SND_SOC_DAIFMT_RIGHT_J:
		ctrl1 |= 2 << 6;
		break;
	case SND_SOC_DAIFMT_LEFT_J:
		ctrl1 |= 3 << 6;
	default:
		return -EINVAL;
	}

	/* Set the clocks inversion */
	switch (fmt & SND_SOC_DAIFMT_INV_MASK) {
	case SND_SOC_DAIFMT_NB_NF:
		ctrl2 &= ~BIT(3);
		break;
	case SND_SOC_DAIFMT_IB_NF:
		ctrl2 |= BIT(3);
		break;
	default:
		return -EINVAL;
	}

	err = dac3100_i2c_write(dac, DAC3100_CODEC_IFACE_CTRL1, ctrl1);
	if (err)
		return err;

	err = dac3100_i2c_write(dac, DAC3100_CODEC_IFACE_CTRL2, ctrl2);
	if (err)
		return err;

	return 0;
}

static int dac3100_reset(struct dac3100 *dac)
{
	int ret;

	ret = dac3100_i2c_write(dac, DAC3100_RESET, 1);
	if (ret != 0) {
		debug("%s: Failed to reset codec: %d\n", __func__, ret);
		return ret;
	}

	return ret;
}

static int dac3100_codec_values(struct dac3100 *dac, const void *blob)
{
	// FIXME
	dac->codec.i2c_bus = 0;
	dac->codec.i2c_dev_addr = 0x18;
	dac->reset_gpio = TEGRA_GPIO(R, 5);
	dac->amp_gpio = TEGRA_GPIO(S, 0);

	return 0;
}

int dac3100_init(const void *blob, int sampling_rate, int mclk_freq,
		int bits_per_sample)
{
	struct dac3100 *dac = &g_dac3100;
	dac3100_codec_values(dac, blob);
	if (i2c_get_chip_for_busnum(dac->codec.i2c_bus, dac->codec.i2c_dev_addr, 1, &dac->i2c) < 0) {
		error("%s: Could not get i2c bus\n", __func__);
		return -1;
	}

	gpio_request(dac->reset_gpio, "dac3100_reset");
	gpio_request(dac->amp_gpio, "dac3100_amp");
	gpio_direction_output(dac->reset_gpio, 1);
	gpio_direction_output(dac->amp_gpio, 1);
	dac3100_reset(dac);

	if (dac3100_set_dai_sysclk(dac, 0, mclk_freq, 0) < 0) {
		error("%s: Could not set sysclk\n", __func__);
		return -1;
	}

	if (dac3100_set_dai_fmt(dac, SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_NB_NF |
			SND_SOC_DAIFMT_CBS_CFS) < 0) {
		error("%s: Could not set fmt\n", __func__);
		return -1;
	}

	if (dac3100_hw_params(dac, 2, sampling_rate, bits_per_sample) < 0) {
		error("%s: Could not set hw_params\n", __func__);
		return -1;
	}

	/* Enable HPL+HPR outputs */
	dac3100_i2c_write(dac, DAC3100_DAC_DATA_PATH_SETUP, 0xd4);
	dac3100_i2c_write(dac, DAC3100_DAC_VOLUME, 0x00);
	dac3100_i2c_write(dac, DAC3100_DAC_LEFT_VOLUME, 0x00);
	dac3100_i2c_write(dac, DAC3100_DAC_RIGHT_VOLUME, 0x00);
	dac3100_i2c_write(dac, DAC3100_DAC_MIXER, 0x44);
	dac3100_i2c_write(dac, DAC3100_HPL_DRIVER, 0x06);
	dac3100_i2c_write(dac, DAC3100_HPR_DRIVER, 0x06);

	dac3100_i2c_write(dac, DAC3100_LEFT_VOL_HPL, 0x92);
	dac3100_i2c_write(dac, DAC3100_RIGHT_VOL_HPR, 0x92);

	dac3100_i2c_write(dac, DAC3100_HP_DRIVER, 0x82);
	return 0;
}
