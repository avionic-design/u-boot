/*
 * Copyright (c) 2010-2013, NVIDIA CORPORATION.  All rights reserved.
 *
 * Portions Copyright (C) 2012 Samsung Electronics
 * Rajeshwari Shinde <rajeshwari.s@samsung.com>
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

#ifndef __ARCH_T114_SOUND_H__
#define __ARCH_T114_SOUND_H__

/* I2S values */
#define I2S_PLL_CLK		4800000
#define I2S_SAMPLING_RATE	48000
#define I2S_BITS_PER_SAMPLE	16
#define I2S_CHANNELS		2
#define I2S_RFS			256
#define I2S_BFS			32

/* I2C values */
#define AUDIO_I2C_BUS		1
#define AUDIO_I2C_REG		0x1c

/* Audio Codec */
#define AUDIO_CODEC		"rt5640"

/*
 * get_sound_i2s_values gets values for I2S parameters from DT
 *
 * @param i2stx_info    I2S transmitter transfer param structure
 * @param blob          FDT blob if enabled else NULL
 */
int get_sound_i2s_values(struct i2stx_info *i2s, const void *blob);

#endif	/* __ARCH_T114_SOUND_H__ */
