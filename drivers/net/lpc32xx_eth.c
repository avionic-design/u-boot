/*
 * Copyright (C) 2008 by NXP Semiconductors
 * Copyright (C) 2011 Avionic Design GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Written by Thierry Reding <thierry.reding@avionic-design.de>, based on
 * code by Kevin Wells.
 */

#include <common.h>
#include <config.h>
#include <malloc.h>

#include <asm/io.h>

#include <asm/arch/clkpwr.h>
#include <asm/arch/eth.h>

struct lpc32xx_eth {
	struct lpc32xx_eth_rx_desc *rx_desc;
	struct lpc32xx_eth_rx_stat *rx_stat;
	struct lpc32xx_eth_tx_desc *tx_desc;
	struct lpc32xx_eth_tx_stat *tx_stat;
	void **rx_buff;
	void **tx_buff;
};

#if defined(CONFIG_MII) || defined(CONFIG_CMD_MII)
static int lpc32xx_mii_read(const char *devname, u8 addr, u8 reg, u16 *value)
{
	struct lpc32xx_eth_regs *eth =
		(struct lpc32xx_eth_regs *)LPC32XX_ETH_BASE;
	unsigned int retries = 250;
	int ret = 0;

	writel((addr << 8) | reg, &eth->mii_address);
	writel(ETH_MII_COMMAND_READ, &eth->mii_command);

	while (retries-- > 0) {
		u32 status = readl(&eth->mii_status);
		if (status & ETH_MII_STATUS_BUSY) {
			udelay(1000);
			continue;
		}

		*value = readl(&eth->mii_rx_data);
		ret = 1;
		break;
	}

	writel(0, &eth->mii_command);

	return ret;
}

static int lpc32xx_mii_write(const char *devname, u8 addr, u8 reg, u16 value)
{
	struct lpc32xx_eth_regs *eth =
		(struct lpc32xx_eth_regs *)LPC32XX_ETH_BASE;
	unsigned int retries = 250;
	int ret = 0;

	writel((addr << 8) | reg, &eth->mii_address);
	writel(value, &eth->mii_tx_data);

	while (retries-- > 0) {
		u32 status = readl(&eth->mii_status);
		if (status & ETH_MII_STATUS_BUSY) {
			udelay(1000);
			continue;
		}

		ret = 1;
	}

	return ret;
}
#endif

static int lpc32xx_eth_setup_phy(struct eth_device *dev)
{
	/* TODO: implement */
	return 0;
}

#define IRAM_BASE 0x08000000
#define IRAM_SIZE 0x00040000

#define MAX_FRAME_SIZE 1536
#define NUM_RX_PACKETS 16
#define NUM_TX_PACKETS 16

static int lpc32xx_eth_setup_buffers(struct eth_device *dev)
{
	struct lpc32xx_eth_regs *eth =
		(struct lpc32xx_eth_regs *)LPC32XX_ETH_BASE;
	struct lpc32xx_eth *priv = dev->priv;
	unsigned long offset = 0;
	unsigned int i;

	priv->rx_desc = (struct lpc32xx_eth_rx_desc *)(IRAM_BASE + offset);
	offset += sizeof(*priv->rx_desc) * NUM_RX_PACKETS;
	priv->rx_stat = (struct lpc32xx_eth_rx_stat *)(IRAM_BASE + offset);
	offset += sizeof(*priv->rx_stat) * NUM_RX_PACKETS;

	priv->tx_desc = (struct lpc32xx_eth_tx_desc *)(IRAM_BASE + offset);
	offset += sizeof(*priv->tx_desc) * NUM_TX_PACKETS;
	priv->tx_stat = (struct lpc32xx_eth_tx_stat *)(IRAM_BASE + offset);
	offset += sizeof(*priv->tx_stat) * NUM_TX_PACKETS;

	for (i = 0; i < NUM_RX_PACKETS; i++) {
		priv->rx_buff[i] = (void *)(IRAM_BASE + offset);
		priv->rx_desc[i].address = IRAM_BASE + offset;
		priv->rx_desc[i].control = ETH_RX_DESC_INT_EN |
		                     (MAX_FRAME_SIZE - 1);
		priv->rx_stat[i].info = 0;
		priv->rx_stat[i].csum = 0;
		offset += MAX_FRAME_SIZE;
	}

	for (i = 0; i < NUM_TX_PACKETS; i++) {
		priv->tx_buff[i] = (void *)(IRAM_BASE + offset);
		priv->tx_desc[i].address = IRAM_BASE + offset;
		priv->tx_desc[i].control = 0;
		priv->tx_stat[i].info = 0;
		offset += MAX_FRAME_SIZE;
	}

	writel((u32)priv->rx_desc, &eth->rx_desc);
	writel((u32)priv->rx_stat, &eth->rx_status);
	writel(NUM_RX_PACKETS - 1, &eth->rx_num);

	writel((u32)priv->tx_desc, &eth->tx_desc);
	writel((u32)priv->tx_stat, &eth->tx_status);
	writel(NUM_TX_PACKETS - 1, &eth->tx_num);

	return 0;
}

static int lpc32xx_eth_init(struct eth_device *dev, bd_t *bd)
{
	struct lpc32xx_clkpwr_regs *clkpwr =
		(struct lpc32xx_clkpwr_regs *)LPC32XX_CLKPWR_BASE;
	struct lpc32xx_eth_regs *eth =
		(struct lpc32xx_eth_regs *)LPC32XX_ETH_BASE;
	u32 data = 0;
	u32 value;

#ifdef CONFIG_LPC32XX_ETH_RMII
	value = CLKPWR_MAC_CTRL_PINS_RMII;
#else
	value = CLKPWR_MAC_CTRL_PINS_MII;
#endif

	writel(CLKPWR_MAC_CTRL_HRCCLK_EN | CLKPWR_MAC_CTRL_MMIOCLK_EN |
			CLKPWR_MAC_CTRL_DMACLK_EN | value, &clkpwr->mac_ctrl);

	writel(ETH_MII_CONFIG_CLK(ETH_MII_CONFIG_CLK_DIV28),
			&eth->mii_config);

	writel(ETH_MAC1_SOFT_RESET | ETH_MAC1_SIMULATION_RESET |
			ETH_MAC1_RESET_MCS_TX | ETH_MAC1_RESET_TX |
			ETH_MAC1_RESET_MCS_RX | ETH_MAC1_RESET_RX,
			&eth->mac1);

	writel(ETH_COMMAND_REG_RESET | ETH_COMMAND_TX_RESET |
			ETH_COMMAND_RX_RESET, &eth->command);
	udelay(10000);

	writel(ETH_MAC1_RX_ALL, &eth->mac1);
	writel(ETH_MAC2_PAD_ENABLE | ETH_MAC2_CRC_ENABLE, &eth->mac2);
	writel(MAX_FRAME_SIZE, &eth->maxf);

	writel(ETH_CLRT_RETRY_MAX(0xf) | ETH_CLRT_COLLISION_WINDOW(0x37),
			&eth->clrt);
	writel(ETH_IPGR_PART2(0x12), &eth->ipgr);

#ifdef CONFIG_LPC32XX_ETH_RMII
	writel(ETH_COMMAND_RMII | ETH_COMMAND_PASS_RUNT_FRAME, &eth->command);
	writel(ETH_SUPP_RESET_RMII, &eth->supp);
	udelay(10000);
#else
	writel(ETH_COMMAND_PASS_RUNT_FRAME, &eth->command);
#endif

	lpc32xx_eth_setup_phy(dev);
	lpc32xx_eth_setup_buffers(dev);

	writel(ETH_RX_FILTER_CTRL_BROADCAST | ETH_RX_FILTER_CTRL_PERFECT,
			&eth->rx_filter_control);

	writel(0xffff, &eth->int_clear);
	writel(0, &eth->int_enable);

	value = readl(&eth->command);
	value |= ETH_COMMAND_RX_ENABLE | ETH_COMMAND_TX_ENABLE;
	writel(value, &eth->command);

	eth_send(&data, sizeof(data));

	return 0;
}

static void lpc32xx_eth_halt(struct eth_device *dev)
{
	struct lpc32xx_clkpwr_regs *clkpwr =
		(struct lpc32xx_clkpwr_regs *)LPC32XX_CLKPWR_BASE;
	struct lpc32xx_eth_regs *eth =
		(struct lpc32xx_eth_regs *)LPC32XX_ETH_BASE;

	writel(ETH_MAC1_SOFT_RESET | ETH_MAC1_SIMULATION_RESET |
			ETH_MAC1_RESET_MCS_TX | ETH_MAC1_RESET_TX |
			ETH_MAC1_RESET_MCS_RX | ETH_MAC1_RESET_RX,
			&eth->mac1);
	writel(ETH_COMMAND_REG_RESET | ETH_COMMAND_TX_RESET |
			ETH_COMMAND_RX_RESET, &eth->command);
	udelay(2000);

#ifdef CONFIG_LPC32XX_ETH_RMII
	writel(CLKPWR_MAC_CTRL_PINS_RMII, &clkpwr->mac_ctrl);
#else
	writel(CLKPWR_MAC_CTRL_PINS_MII, &clkpwr->mac_ctrl);
#endif
}

static int lpc32xx_eth_send(struct eth_device *dev, volatile void *packet,
		int length)
{
	struct lpc32xx_eth_regs *eth =
		(struct lpc32xx_eth_regs *)LPC32XX_ETH_BASE;
	struct lpc32xx_eth *priv = dev->priv;
	u32 consumer = readl(&eth->tx_consumer_index);
	u32 producer = readl(&eth->tx_producer_index);

	if (producer == ((consumer + 1) % NUM_TX_PACKETS)) {
		printf("TX buffer full\n");
		return 0;
	}

	priv->tx_desc[producer].control = ETH_TX_DESC_LAST | length;
	memcpy(priv->tx_buff[producer], (const void *)packet, length);

	if (++producer >= NUM_TX_PACKETS)
		producer = 0;

	writel(producer, &eth->tx_producer_index);

	/* TODO: wait for buffer to complete */

	return 0;
}

static int lpc32xx_eth_recv(struct eth_device *dev)
{
	struct lpc32xx_eth_regs *eth =
		(struct lpc32xx_eth_regs *)LPC32XX_ETH_BASE;
	struct lpc32xx_eth *priv = dev->priv;
	int length = 0;
	u32 producer;
	u32 consumer;

	producer = readl(&eth->rx_producer_index);
	consumer = readl(&eth->rx_consumer_index);

	if (producer != consumer) {
		writel(ETH_INT_RX_DONE, &eth->int_clear);

		length = priv->rx_stat[consumer].info & 0x7ff;

		if (length > 0) {
			void *packet = (void *)NetRxPackets[0];
			memcpy(packet, priv->rx_buff[consumer], length);
			NetReceive(packet, length);
		}

		if (++consumer >= NUM_TX_PACKETS)
			consumer = 0;

		writel(consumer, &eth->rx_consumer_index);
	}

	return length;
}

static int lpc32xx_eth_setup_addr(struct eth_device *dev)
{
	struct lpc32xx_clkpwr_regs *clkpwr =
		(struct lpc32xx_clkpwr_regs *)LPC32XX_CLKPWR_BASE;
	struct lpc32xx_eth_regs *eth =
		(struct lpc32xx_eth_regs *)LPC32XX_ETH_BASE;

	/* Make sure the clocks are enabled */
	writel(readl(&clkpwr->mac_ctrl) |
	       CLKPWR_MAC_CTRL_HRCCLK_EN |
	       CLKPWR_MAC_CTRL_MMIOCLK_EN |
	       CLKPWR_MAC_CTRL_DMACLK_EN,
	       &clkpwr->mac_ctrl);

	writel((dev->enetaddr[1] << 8) | dev->enetaddr[0], &eth->sa[2]);
	writel((dev->enetaddr[3] << 8) | dev->enetaddr[2], &eth->sa[1]);
	writel((dev->enetaddr[5] << 8) | dev->enetaddr[4], &eth->sa[0]);

	return 0;
}

int lpc32xx_eth_initialize(bd_t *bd)
{
	struct eth_device *dev;

	dev = malloc(sizeof(*dev) + sizeof(struct lpc32xx_eth));
	if (!dev)
		return -1;

	memset(dev, 0, sizeof(*dev));
	strcpy(dev->name, "lpc32xx");

	dev->iobase = 0;
	dev->priv = (void *)dev + sizeof(struct lpc32xx_eth);
	dev->init = lpc32xx_eth_init;
	dev->halt = lpc32xx_eth_halt;
	dev->send = lpc32xx_eth_send;
	dev->recv = lpc32xx_eth_recv;
	dev->write_hwaddr = lpc32xx_eth_setup_addr;

	eth_register(dev);

#if defined(CONFIG_MII) || defined(CONFIG_CMD_MII)
	miiphy_register(dev->name, lpc32xx_mii_read, lpc32xx_mii_write);
#endif

	return 0;
}
