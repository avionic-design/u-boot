/*
 * tlv320dac3100.h -- Codec driver for TI TLV320DAC3100
 *
 * Copyright (c) 2015 Avionic Design GmbH
 *
 * Authors: Julian Scheel <julian@jusst.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef _TLV320DAC3100_H_
#define _TLV320DAC3100_H_

int dac3100_init(const void *blob, int sampling_rate, int mclk_freq,
		int bits_per_sample);

#endif
