#ifndef LPC32XX_MLC_H
#define LPC32XX_MLC_H 1

#include <asm/types.h>

#define LPC32XX_MLC_BASE 0x200A8000

struct lpc32xx_mlc_regs {
	u32 buffer[8192];
	u32 data[8192];
	u32 command;
	u32 address;
	u32 ecc_enc;
	u32 ecc_dec;
	u32 ecc_auto_enc;
	u32 ecc_auto_dec;
	u32 ecc_rp;
	u32 ecc_wp;
	u32 rubp;
	u32 robp;
	u32 sw_wp_lo;
	u32 sw_wp_hi;
	u32 icr;
	u32 timing;
	u32 int_mask;
	u32 int_status;
	u32 lock;
	u32 status;
	u32 ceh;
};

#define MLC_TIMING_TCEA(x) (((x) & 0x3) << 24)
#define MLC_TIMING_TWBTRB(x) (((x) & 0x1f) << 19)
#define MLC_TIMING_TRHZ(x) (((x) & 0x7) << 16)
#define MLC_TIMING_TREH(x) (((x) & 0xf) << 12)
#define MLC_TIMING_TRP(x) (((x) & 0xf) << 8)
#define MLC_TIMING_TWH(x) (((x) & 0xf) << 4)
#define MLC_TIMING_TWP(x) (((x) & 0xf) << 0)

#define MLC_ICR_BUS_WIDTH_16 (1 << 0)
#define MLC_ICR_EXTRA_CYCLES (1 << 1)
#define MLC_ICR_LARGE_BLOCK  (1 << 2)
#define MLC_ICR_SW_WP        (1 << 4)

#define MLC_STATUS_NAND_READY (1 << 0)
#define MLC_STATUS_CTRL_READY (1 << 1)
#define MLC_STATUS_ECC_READY (1 << 2)

#define MLC_CEH_NORMAL (1 << 0)

#define NAND_CMD_PAGE_READ1  0x00
#define NAND_CMD_PAGE_WRITE2 0x10
#define NAND_CMD_ERASE1      0x60
#define NAND_CMD_STATUS      0x70
#define NAND_CMD_PAGE_WRITE1 0x80
#define NAND_CMD_READ_ID     0x90
#define NAND_CMD_ERASE2      0xD0
#define NAND_CMD_RESET       0xff

#endif /* LPC32XX_MLC_H */
