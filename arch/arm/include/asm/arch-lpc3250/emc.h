#ifndef LPC32XX_EMC_H
#define LPC32XX_EMC_H 1

#include <asm/types.h>

#define LPC32XX_EMC_BASE 0x31080000

struct lpc32xx_emc_static_regs {
	u32 config;
	u32 waitwen;
	u32 wait0en;
	u32 waitrd;
	u32 page;
	u32 wr;
	u32 turn;
	u32 reserved;
};

struct lpc32xx_emc_ahb_regs {
	u32 control;
	u32 status;
	u32 timeout;
	u32 reserved[5];
};

#define EMC_AHB_CONTROL_PORTBUFF_EN (1 << 0)

struct lpc32xx_emc_regs {
	u32 control;
	u32 status;
	u32 config;
	u32 reserved1[5];
	u32 dyn_control;
	u32 dyn_refresh;
	u32 dyn_read_cfg;
	u32 reserved2;
	u32 dyn_trp;
	u32 dyn_tras;
	u32 dyn_tsrex;
	u32 reserved3[2];
	u32 dyn_twr;
	u32 dyn_trc;
	u32 dyn_trfc;
	u32 dyn_txsr;
	u32 dyn_trrd;
	u32 dyn_tmrd;
	u32 dyn_tcdlr;
	u32 reserved4[8];
	u32 stc_ext_wait;
	u32 reserved5[31];
	u32 dyn_config0;
	u32 dyn_rascas0;
	u32 reserved6[6];
	u32 dyn_config1;
	u32 dyn_rascas1;
	u32 reserved7[54];
	struct lpc32xx_emc_static_regs stc[4];
	u32 reserved8[96];
	struct lpc32xx_emc_ahb_regs ahb[5];
};

#define EMC_CONTROL_DYN_SDRAM_CTRL_EN (1 << 0)

#define EMC_STC_BLS_EN (1 << 7)
#define EMC_STC_MEMWIDTH_16 1

#define EMC_DYN_CLKEN_ALWAYS_ON (1 << 0)
#define EMC_DYN_CLK_ALWAYS_ON (1 << 1)
#define EMC_DYN_DIS_MEMCLK_IN_SFRSH (1 << 3)
#define EMC_DYN_REFRESH_IVAL(n) (((n) >> 4) & 0x7ff)
#define EMC_DYN_NORMAL_MODE 0x000
#define EMC_DYN_CMD_MODE 0x080
#define EMC_DYN_PALL_MODE 0x100
#define EMC_DYN_NOP_MODE 0x180

#endif /* LPC32XX_EMC_H */
