#include <common.h>
#include <malloc.h>

#include <asm/io.h>
#include <asm/errno.h>
#include <asm/arch/clkpwr.h>
#include <asm/arch/mlc.h>

#define BIT(x) (1 << (x))

static int mlc_init(void)
{
	struct lpc32xx_clkpwr_regs *clkpwr =
		(struct lpc32xx_clkpwr_regs *)LPC32XX_CLKPWR_BASE;
	struct lpc32xx_mlc_regs *mlc =
		(struct lpc32xx_mlc_regs *)LPC32XX_MLC_BASE;
	u32 value;

	value = readl(&clkpwr->nand_clk_ctrl);
	value &= ~CLKPWR_NAND_CLK_CTRL_SLC_ENABLE;
	value &= ~CLKPWR_NAND_CLK_CTRL_SLC_SELECT;
	value |=  CLKPWR_NAND_CLK_CTRL_MLC_ENABLE;
	writel(value, &clkpwr->nand_clk_ctrl);

	writel(0x0000a25e, &mlc->lock);
	writel(MLC_ICR_EXTRA_CYCLES, &mlc->icr);

	writel(LPC32XX_MLC_NAND_TIMING, &mlc->timing);

	writel(0, &mlc->int_mask);

	writel(MLC_CEH_NORMAL, &mlc->ceh);

	writel(NAND_CMD_RESET, &mlc->command);

	while (1) {
		value = readl(&mlc->status);
		if (value & MLC_STATUS_NAND_READY)
			break;
	}

	writel(1, &mlc->rubp);

	return 0;
}

static int mlc_exit(void)
{
	struct lpc32xx_clkpwr_regs *clkpwr =
		(struct lpc32xx_clkpwr_regs *)LPC32XX_CLKPWR_BASE;
	u32 value;

	value = readl(&clkpwr->nand_clk_ctrl);
	value &= ~CLKPWR_NAND_CLK_CTRL_MLC_ENABLE;
	value |=  CLKPWR_NAND_CLK_CTRL_SLC_ENABLE;
	value |=  CLKPWR_NAND_CLK_CTRL_SLC_SELECT;
	writel(value, &clkpwr->nand_clk_ctrl);

	return 0;
}

static int erase_block(unsigned long offset)
{
	struct lpc32xx_mlc_regs *mlc =
		(struct lpc32xx_mlc_regs *)LPC32XX_MLC_BASE;
	u32 status;

	writel(NAND_CMD_ERASE1, &mlc->command);

	writel(0, &mlc->address);
	writel(0, &mlc->address);
#ifdef CONFIG_SYS_NAND_4_ADDR_CYCLE
	writel(0, &mlc->address);
#endif

	writel(NAND_CMD_ERASE2, &mlc->command);

	while (1) {
		status = readl(&mlc->status);
		if (status & MLC_STATUS_NAND_READY)
			break;
	}

	writel(NAND_CMD_STATUS, &mlc->command);
	status = readl(&mlc->data[0]);

	if (status & 0x1)
		return -EIO;

	return 0;
}

static int write_page(unsigned long offset, const void *buffer, size_t size)
{
	unsigned int page = offset / CONFIG_SYS_NAND_PAGE_SIZE;
	unsigned int block = page / CONFIG_SYS_NAND_PAGE_COUNT;
	struct lpc32xx_mlc_regs *mlc =
		(struct lpc32xx_mlc_regs *)LPC32XX_MLC_BASE;
	const u8 *ptr = buffer;
	u32 address;
	u32 status;
	size_t i;

	writel(MLC_CEH_NORMAL, &mlc->ceh);
	writel(NAND_CMD_PAGE_WRITE1, &mlc->command);

	address = (block & 0xfff) << 13 | (page & 0x1f) << 8;

	writel((address >>  0) & 0xff, &mlc->address);
	writel((address >>  8) & 0xff, &mlc->address);
	writel((address >> 16) & 0xff, &mlc->address);
#ifdef CONFIG_SYS_NAND_4_ADDR_CYCLE
	writel((address >> 24) & 0xff, &mlc->address);
#endif

	writel(1, &mlc->ecc_enc);

	for (i = 0; i < size; i += 4) {
		u32 data = ptr[i + 3] << 24 | ptr[i + 2] << 16 |
		           ptr[i + 1] <<  8 | ptr[i + 0];
		writel(data, &mlc->data[0]);
	}

	writel(0xffffffff, &mlc->data[0]);
	writew(0xffff, &mlc->data[0]);

	writel(1, &mlc->ecc_wp);

	while (1) {
		status = readl(&mlc->status);
		if (status & MLC_STATUS_CTRL_READY)
			break;
	}

	writel(NAND_CMD_PAGE_WRITE2, &mlc->command);
	writel(0, &mlc->ceh);

	while (1) {
		status = readl(&mlc->status);
		if (status & MLC_STATUS_NAND_READY)
			break;
	}

	writel(NAND_CMD_STATUS, &mlc->command);
	status = readl(&mlc->data[0]);

	if (status & 0x1)
		return -EIO;

	return 0;
}

static int read_page(unsigned long offset, void *buffer, size_t size)
{
	unsigned int page = offset / CONFIG_SYS_NAND_PAGE_SIZE;
	unsigned int block = page / CONFIG_SYS_NAND_PAGE_COUNT;
	struct lpc32xx_mlc_regs *mlc =
		(struct lpc32xx_mlc_regs *)LPC32XX_MLC_BASE;
	u8 *ptr = buffer;
	u32 address;
	u32 status;
	size_t i;

	writel(MLC_CEH_NORMAL, &mlc->ceh);
	writel(NAND_CMD_PAGE_READ1, &mlc->command);

	address = (block & 0xfff) << 13 | (page & 0x1f) << 8;

	writel((address >>  0) & 0xff, &mlc->address);
	writel((address >>  8) & 0xff, &mlc->address);
	writel((address >> 16) & 0xff, &mlc->address);
#ifdef CONFIG_SYS_NAND_4_ADDR_CYCLE
	writel((address >> 24) & 0xff, &mlc->address);
#endif

	while (1) {
		status = readl(&mlc->status);
		if (status & MLC_STATUS_NAND_READY)
			break;
	}

	writel(1, &mlc->ecc_dec);

	for (i = 0; i < size; i += 4) {
		u32 data = readl(&mlc->data[0]);
		ptr[i + 0] = (data >>  0) & 0xff;
		ptr[i + 1] = (data >>  8) & 0xff;
		ptr[i + 2] = (data >> 16) & 0xff;
		ptr[i + 3] = (data >> 24) & 0xff;
	}

	(void)readl(&mlc->data[0]);
	(void)readw(&mlc->data[0]);

	writel(1, &mlc->ecc_rp);

	while (1) {
		status = readl(&mlc->status);
		if (status & MLC_STATUS_ECC_READY)
			break;
	}

	writel(0, &mlc->ceh);

	while (1) {
		status = readl(&mlc->status);
		if (status & MLC_STATUS_NAND_READY)
			break;
	}

	return 0;
}

/*
 * The update procedure is taken from the LPC3250 User Manual (UM10326),
 * Chapter 35, Section 2.2.3.
 */
int update_kickstart(const void *buffer, size_t size)
{
	size_t pagesize = CONFIG_SYS_NAND_PAGE_SIZE;
	size_t oobsize = CONFIG_SYS_NAND_OOBSIZE;
	size_t count = 0;
	u8 icr = 0;
	void *page;
	u8 pages;
	u8 *ptr;
	int err;
	int i;

	page = malloc(pagesize + oobsize);
	if (!page) {
		printf("memory allocation failed\n");
		return -ENOMEM;
	}

	pages = DIV_ROUND_UP(size, pagesize) + 1;
	memset(page, 0, pagesize + oobsize);
	ptr = page;

	icr |= BIT(4); /* 8-bit interface */

#ifdef CONFIG_SYS_NAND_4_ADDR_CYCLE
	icr |= BIT(1); /* 4/5 address cycles */
#else
	icr |= BIT(5);
#endif

#if CONFIG_SYS_NAND_PAGE_SIZE == 2048
	icr |= BIT(2); /* large page */
#else
	icr |= BIT(6); /* small page */
#endif

	icr |= BIT(7);

	/* write 4 instances of the ICR data */
	for (i = 0; i < 4; i++)
		ptr[i * 4] = icr;

	for (i = 0; i < 4; i++) {
		ptr[16 + (i * 8) + 0] =  pages;
		ptr[16 + (i * 8) + 4] = ~pages;
	}

	ptr[48] = 0xaa;

	mlc_init();

	err = erase_block(0);
	if (err < 0)
		goto out;

	err = write_page(0, page, pagesize);
	if (err < 0)
		goto out;

	while (count < size) {
		write_page(0x200 + count, buffer + count, pagesize);
		count += pagesize;
	}

out:
	mlc_exit();
	free(page);
	return err;
}

ssize_t load_kickstart(void *buffer, size_t size)
{
	size_t pagesize = CONFIG_SYS_NAND_PAGE_SIZE;
	size_t count = 0;
	ssize_t ret = 0;
	int err;

	mlc_init();

	while (count < size) {
		err = read_page(0x200 + count, buffer + count, pagesize);
		if (err < 0) {
			ret = err;
			break;
		}

		count += pagesize;
	}

	if (ret == 0)
		ret = count;

	mlc_exit();

	return ret;
}

static int do_kickstart(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	int ret = 0;

	if (argc < 2)
		return cmd_usage(cmdtp);

	if (strcmp(argv[1], "update") == 0) {
		char *env_addr = getenv("loadaddr");
		char *env_size = getenv("filesize");
		unsigned long size = 16384;
		unsigned long addr;

		if (argc >= 3)
			env_addr = argv[2];

		if (argc >= 4)
			env_size = argv[3];

		if (!env_addr) {
			printf("no address specified\n");
			return 1;
		}

		if (!env_size) {
			printf("no size specified\n");
			return 1;
		}

		addr = simple_strtoul(env_addr, NULL, 16);
		size = simple_strtoul(env_size, NULL, 16);

		printf("burning kickstart from %#08lx (%lu bytes)\n",
				addr, size);

		update_kickstart((const void *)addr, size);
	}

	if (strcmp(argv[1], "load") == 0) {
		char *env_addr = getenv("loadaddr");
		unsigned long size = 16384;
		unsigned long addr;

		if (argc >= 3)
			env_addr = argv[2];

		if (!env_addr) {
			printf("no address specified\n");
			return 1;
		}

		addr = simple_strtoul(env_addr, NULL, 16);

		if (argc >= 4)
			size = simple_strtoul(argv[3], NULL, 16);

		printf("loading kickstart from NAND to address %#08lx (%lu "
				"bytes)...\n", addr, size);
		load_kickstart((void *)addr, size);
	}

	return ret;
}

U_BOOT_CMD(kick, 4, 0, do_kickstart,
	"LPC32xx kickstart support",
	"[operation] [image address] [image size]\n"
	"operations:\n"
	"  update\t[address] [size]\t\t\tLoad kickstart from memory"
);
