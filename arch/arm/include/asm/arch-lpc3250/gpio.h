#ifndef LPC32XX_GPIO_H
#define LPC32XX_GPIO_H 1

#define LPC32XX_GPIO_BASE 0x40028000

struct lpc32xx_gpio_regs {
	u32 p3_in;
	u32 p3_out_set;
	u32 p3_out_clr;
	u32 p3_out;
	u32 p2_dir_set;
	u32 p2_dir_clr;
	u32 p2_dir;
	u32 p2_in;
	u32 p2_out_set;
	u32 p2_out_clr;
	u32 p2_mux_set;
	u32 p2_mux_clr;
	u32 p2_mux;
	u32 reserved1[3];
	u32 p0_in;
	u32 p0_out_set;
	u32 p0_out_clr;
	u32 p0_out;
	u32 p0_dir_set;
	u32 p0_dir_clr;
	u32 p0_dir;
	u32 reserved2;
	u32 p1_in;
	u32 p1_out_set;
	u32 p1_out_clr;
	u32 p1_out;
	u32 p1_dir_set;
	u32 p1_dir_clr;
	u32 p1_dir;
	u32 reserved3;
	u32 reserved4[32];
	u32 p_mux_set;
	u32 p_mux_clr;
	u32 p_mux;
	u32 reserved5;
	u32 p3_mux_set;
	u32 p3_mux_clr;
	u32 p3_mux;
	u32 reserved6;
	u32 p0_mux_set;
	u32 p0_mux_clr;
	u32 p0_mux;
	u32 reserved7;
	u32 p1_mux_set;
	u32 p1_mux_clr;
	u32 p1_mux;
};

#define P_I2STXSDA1_MAT31 (1 << 2)
#define P_I2STXCLK1_MAT30 (1 << 3)
#define P_I2STXWS1_CAP30 (1 << 4)
#define P_SPI2DATAIO_MOSI1 (1 << 5)
#define P_SPI2DATAIN_MISO1 (1 << 6)
#define P_SPI2CLK_SCK1 (1 << 8)
#define P_MAT21 (1 << 13)
#define P_MAT20 (1 << 14)
#define P_MAT03 (1 << 17)
#define P_MAT02 (1 << 18)
#define P_MAT01 (1 << 19)
#define P_MAT00 (1 << 20)

#define P1_ALL 0x00ffffff

#define P2_GPIO02_KEYROW6 (1 << 0)
#define P2_GPIO03_KEYROW7 (1 << 1)
#define P2_GPO21_U4TX (1 << 2)
#define P2_SDRAMD19D31_GPIO (1 << 3)
#define P2_GPIO04_SSEL1 (1 << 4)
#define P2_GPIO05_SSEL0 (1 << 5)
#define P_SPI1DATAIO_SSP0_MOSI (1 << 9)
#define P_SPI1DATAIN_SSP0_MISO (1 << 10)
#define P_SPI1CLK_SCK0 (1 << 12)
#define P_U7TX_MAT11 (1 << 15)

#define P3_GPO2_MAT10 (1 << 2)
#define P3_GPO6 (1 << 6)
#define P3_GPO8 (1 << 8)
#define P3_GPO9 (1 << 9)
#define P3_GPO10_MC2B (1 << 10)
#define P3_GPO12_MC2A (1 << 12)
#define P3_GPO13_MC1B (1 << 13)
#define P3_GPO15_MC1A (1 << 15)
#define P3_GPO16_MC0B (1 << 16)
#define P3_GPO18_MC0A (1 << 17)

#define P0_GPO0_I2SRXCLK1 (1 << 0)
#define P0_GPO1_I2SRXWS1 (1 << 1)
#define P0_GPO2_I2SRXSDA0 (1 << 2)
#define P0_GPO3_I2SRXCLK0 (1 << 3)
#define P0_GPO4_I2SRXWS0 (1 << 4)
#define P0_GPO5_I2STXSDA0 (1 << 5)
#define P0_GPO6_I2STXCLK0 (1 << 6)
#define P0_GPO7_I2STXWS0 (1 << 7)

#define P2_DIR_GPIO(pin) (1 << ((pin) + 25))
#define P3_GPO(pin) (1 << (pin))

#endif /* LPC32XX_GPIO_H */
