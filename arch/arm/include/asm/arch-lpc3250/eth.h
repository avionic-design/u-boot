#ifndef LPC32XX_ETH_H
#define LPC32XX_ETH_H 1

#define LPC32XX_ETH_BASE 0x31060000

struct lpc32xx_eth_regs {
	u32 mac1;
	u32 mac2;
	u32 ipgt;
	u32 ipgr;
	u32 clrt;
	u32 maxf;
	u32 supp;
	u32 test;
	u32 mii_config;
	u32 mii_command;
	u32 mii_address;
	u32 mii_tx_data;
	u32 mii_rx_data;
	u32 mii_status;
	u32 reserved1[2];
	u32 sa[3];
	u32 reserved2[45];
	u32 command;
	u32 status;
	u32 rx_desc;
	u32 rx_status;
	u32 rx_num;
	u32 rx_producer_index;
	u32 rx_consumer_index;
	u32 tx_desc;
	u32 tx_status;
	u32 tx_num;
	u32 tx_producer_index;
	u32 tx_consumer_index;
	u32 reserved3[10];
	u32 tsv0;
	u32 tsv1;
	u32 rsv;
	u32 reserved4[3];
	u32 flow_control_counter;
	u32 flow_control_status;
	u32 reserved5[34];
	u32 rx_filter_control;
	u32 rx_filter_wol_status;
	u32 rx_filter_wol_clear;
	u32 reserved6;
	u32 hash_filter_low;
	u32 hash_filter_high;
	u32 reserved7[882];
	u32 int_status;
	u32 int_enable;
	u32 int_clear;
	u32 int_set;
	u32 reserved8;
	u32 power_down;
	u32 reserved9;
};

#define ETH_MAC1_RX_ENABLE (1 << 0)
#define ETH_MAC1_RX_ALL (1 << 1)
#define ETH_MAC1_RESET_TX (1 << 8)
#define ETH_MAC1_RESET_MCS_TX (1 << 9)
#define ETH_MAC1_RESET_RX (1 << 10)
#define ETH_MAC1_RESET_MCS_RX (1 << 11)
#define ETH_MAC1_SIMULATION_RESET (1 << 14)
#define ETH_MAC1_SOFT_RESET (1 << 15)

#define ETH_MAC2_CRC_ENABLE (1 << 4)
#define ETH_MAC2_PAD_ENABLE (1 << 5)

#define ETH_COMMAND_RX_ENABLE (1 << 0)
#define ETH_COMMAND_TX_ENABLE (1 << 1)
#define ETH_COMMAND_REG_RESET (1 << 3)
#define ETH_COMMAND_TX_RESET (1 << 4)
#define ETH_COMMAND_RX_RESET (1 << 5)
#define ETH_COMMAND_PASS_RUNT_FRAME (1 << 6)

#define ETH_CLRT_RETRY_MAX(n) ((n) & 0xf)
#define ETH_CLRT_COLLISION_WINDOW(n) (((n) & 0x3f) << 8)

#define ETH_IPGR_PART2(n) ((n) & 0x7f)

#define ETH_RX_FILTER_CTRL_UNICAST (1 << 0)
#define ETH_RX_FILTER_CTRL_BROADCAST (1 << 1)
#define ETH_RX_FILTER_CTRL_MULTICAST (1 << 2)
#define ETH_RX_FILTER_CTRL_PERFECT (1 << 5)

#define ETH_MII_CONFIG_CLK(n) (((n) & 0x7) << 2)
#define ETH_MII_CONFIG_CLK_DIV4 0
#define ETH_MII_CONFIG_CLK_DIV6 2
#define ETH_MII_CONFIG_CLK_DIV8 3
#define ETH_MII_CONFIG_CLK_DIV10 4
#define ETH_MII_CONFIG_CLK_DIV14 5
#define ETH_MII_CONFIG_CLK_DIV20 6
#define ETH_MII_CONFIG_CLK_DIV28 7

#define ETH_MII_COMMAND_READ (1 << 0)

#define ETH_MII_STATUS_BUSY (1 << 0)

#define ETH_INT_RX_DONE (1 << 3)

struct lpc32xx_eth_rx_desc {
	u32 address;
	u32 control;
};

#define ETH_RX_DESC_INT_EN (1 << 31)

struct lpc32xx_eth_rx_stat {
	u32 info;
	u32 csum;
};

struct lpc32xx_eth_tx_desc {
	u32 address;
	u32 control;
};

#define ETH_TX_DESC_LAST (1 << 30)

struct lpc32xx_eth_tx_stat {
	u32 info;
};

#endif /* LPC32XX_ETH_H */
