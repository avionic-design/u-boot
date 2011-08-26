#ifndef LPC32XX_GPIO_H
#define LPC32XX_GPIO_H 1

#define LPC32XX_GPIO 0x40028000

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

#endif /* LPC32XX_GPIO_H */
