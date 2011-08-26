/*
 * Copyright (C) 2011 Avionic Design GmbH
 * All rights reserved.
 *
 * @Author: Thierry Reding <thierry.reding@avionic-design.de>
 * @Descr: Avionic Design PT-IP board configuration file
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __PTIP_H__
#define __PTIP_H__

/*
 * Ethernet buffer support in uncached IRAM and buffer size
 */
#define USE_IRAM_FOR_ETH_BUFFERS
#define IRAM_ETH_BUFF_BASE 0x08010000 /* Uncached IRAM */
#define IRAM_ETH_BUFF_SIZE 0x00010000

/*
 * Linux machine type
 */
#define MACH_TYPE_PTIP (4000)
#define MACH_TYPE_UBOOTSYS MACH_TYPE_PTIP

/*
 * System UART selection, valid selections include UART3, UART4,
 * UART5, and UART6
 */
#define CFG_UART_SEL UART5

/*
 * SDRAM physical Memory Map
 */
#define CONFIG_NR_DRAM_BANKS    1
#define PHYS_SDRAM_1		0x80000000 /* SDRAM Bank #1 */

#define CONFIG_SYS_SDRAM_BASE	PHYS_SDRAM_1
#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_SDRAM_BASE + 0x1000 - GENERATED_GBL_DATA_SIZE)


/*
 * NOR FLASH not supported
 */
#define CONFIG_SYS_NO_FLASH

/*
 * Address and size of Environment Data
 */
#define CONFIG_ENV_IS_IN_NAND	1
#define CONFIG_ENV_SIZE		0x00008000 /* 2 blocks */
#define CONFIG_ENV_OFFSET	0x00038000 /* Blocks 8/9  */
#define CONFIG_ENV_ADDR		0x80000100 /* Passed to kernel here */

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE

/*
 * 1KHz clock tick
 */
#define CONFIG_SYS_HZ		1000

/*
 * ARM926ejs options
 */
#define CONFIG_ARM926EJS	1 /* This is an arm926ejs CPU core  */
#define CONFIG_SYS_DCACHE_OFF	1

/*
 *
 * u-boot specific options
 *
 */

/*
 * Area and size for malloc
 */
#define CONFIG_SYS_MALLOC_LEN (CONFIG_ENV_SIZE + 128 * 1024)
#define CONFIG_SYS_GBL_DATA_SIZE 128

/*
 * Stack sizes
 *
 * The stack sizes are set up in start.S using the settings below
 */
#define CONFIG_STACKSIZE (32*1024) /* 32K stack */

/*
 * ATAG support
 */
#define CONFIG_CMDLINE_TAG		1
#define CONFIG_SETUP_MEMORY_TAGS	1
#define CONFIG_INITRD_TAG		1

/*
 * Default baud rate and baud rate table, console config
 */
#define CONFIG_CONS_INDEX	   1
#define CONFIG_BAUDRATE		115200
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }

/*
 * Default load address for programs
 */
#define CONFIG_SYS_LOAD_ADDR	0x80800000 /* default load address */

/*
 * Default boot delay is 3 seconds
 */
#define CONFIG_BOOTDELAY 3
#define CONFIG_ZERO_BOOTDELAY_CHECK	/* check for keypress on bootdelay==0 */

/*
 * Interrupts are not supported in this boot loader
 */
#undef CONFIG_USE_IRQ

/*
 * Use verbose help
 */
#define CONFIG_SYS_LONGHELP

/*
 * Command line configuration.
 */
#include <config_cmd_default.h>

#define CONFIG_CMD_DHCP
#define CONFIG_CMD_ENV
#define CONFIG_CMD_ECHO /* ECHO command */
#define CONFIG_CMD_CACHE /* Cache support */
#define CONFIG_CMD_RUN
#define CONFIG_CMD_LOADB
#define CONFIG_CMD_LOADS
#define CONFIG_CMD_SAVES
#define CONFIG_CMD_MEMORY
#define CONFIG_CMD_MTDPARTS
#define CONFIG_CMD_PING
#define CONFIG_CMD_NET
#define CONFIG_CMD_BDI
#define CONFIG_CMD_SAVEENV

/*
#define CONFIG_CMD_UBI
#define CONFIG_CMD_UBIFS
*/

/*
 * USB Host (OHCI) Support
 */
#define CONFIG_CMD_USB
#define CONFIG_USB_OHCI_NEW 1
#define LITTLEENDIAN
#define CONFIG_SYS_USB_OHCI_REGS_BASE 0x31020000
#define CONFIG_SYS_USB_OHCI_SLOT_NAME "lpc3250"
#define CONFIG_SYS_USB_OHCI_MAX_ROOT_PORTS 15
#define CONFIG_USB_STORAGE 1
#define CONFIG_SYS_USB_OHCI_BOARD_INIT

#define CONFIG_DOS_PARTITION 1
#define CONFIG_MAC_PARTITION 1

/*
 * Prompt, command buffer
 */
#define	CONFIG_SYS_CBSIZE		256		/* Console I/O Buffer Size	*/
#define	CONFIG_SYS_PROMPT	"uboot> "	/* Monitor Command Prompt	*/
#define	CONFIG_SYS_PBSIZE (CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16) /* Print Buffer Size */
#define	CONFIG_SYS_MAXARGS		16		/* max number of command args	*/
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE	/* Boot Argument Buffer Size	*/

/*
 * Default range for the memory tests
 */
#define CONFIG_SYS_MEMTEST_START 0x80010000
#define CONFIG_SYS_MEMTEST_END 0x81000000

/*
 * Support for NAND FLASH, environment store in NAND at block 100
 */
#define CONFIG_CMD_NAND
#define CONFIG_SYS_LPC32XX_NAND    /* Enable SLC NAND controller driver */
#define CONFIG_SYS_MAX_NAND_DEVICE 1
#define CONFIG_SYS_NAND_BASE 0x20020000 /* SLC NAND controller */
#define LPC32XX_SLC_NAND_TIMING (SLCTAC_WDR(14) | \
                    SLCTAC_WWIDTH(3) | \
                    SLCTAC_WHOLD(2) | \
                    SLCTAC_WSETUP(2) | \
                    SLCTAC_RDR(14) | \
                    SLCTAC_RWIDTH(1) | \
                    SLCTAC_RHOLD(2) | \
                    SLCTAC_RSETUP(2))

/*
 * NAND H/W ECC specific settings
 */
#define CONFIG_SYS_LPC32XX_DMA            /* DMA support required */
#define CONFIG_SYS_NAND_PAGE_SIZE     512
#define CONFIG_SYS_NAND_ECCSIZE       512 /* ECC generated per page */
#define CONFIG_SYS_NAND_ECCBYTES        6 /* 3 Bytes ECC per 256 bytes */
#define CONFIG_SYS_NAND_OOBSIZE        16 /* OOB size in bytes */

#ifdef CONFIG_CMD_MTDPARTS
#  define CONFIG_MTD_DEVICE	1
#  define CONFIG_MTD_PARTITIONS	1
#endif

#if defined(CONFIG_CMD_UBI) || defined(CONFIG_CMD_UBIFS)
#  define CONFIG_LZO		1
#  define CONFIG_RBTREE		1
#endif

#define MTDIDS_DEFAULT "nand0=flash"
#define MTDPARTS_DEFAULT "mtdparts=flash:224k(spl),32k(environment),512k(uboot),8m(uimage),-(rootfs)"

/*
 * Support for various capabilities
 */
#define CONFIG_AUTO_COMPLETE
#define CONFIG_CMDLINE_EDITING
#define CONFIG_SYS_LOADS_BAUD_CHANGE

/*
 * Network setup
 */
#define CONFIG_NETMASK		255.255.255.0
#define CONFIG_IPADDR		192.168.1.101
#define CONFIG_SERVERIP		192.168.1.41
#define CONFIG_ETHADDR		00:1a:f1:00:00:00
#define CONFIG_GATEWAYIP	192.168.1.1

#define CONFIG_BOOTFILE		uImage
#define CONFIG_LOADADDR		0x80800000
#define CONFIG_ROOTPATH		/home/user/ltib/rootfs

#define BOOTARGS "bootargs=console=ttyS0,115200n8\0"

#define BOOT_NAND "nandboot="					\
	"nand read ${loadaddr} 0x00080000 0x00800000\0"

#define UPDATE_UBOOT "update_uboot="				\
	"nand erase 0x00040000 0x00040000;"			\
	"nand write ${loadaddr} 0x00040000 0x00040000;\0"

#define UPDATE_UIMAGE "update_uimage="				\
	"nand erase 0x00080000 0x00800000;"			\
	"nand write ${loadaddr} 0x00080000 0x00800000;\0"

/*
 * Other preset environment variables and example bootargs string
 */
#define CONFIG_EXTRA_ENV_SETTINGS				\
	BOOTARGS						\
	BOOT_NAND						\
	UPDATE_UBOOT						\
	UPDATE_UIMAGE

/* Default boot command */
#define CONFIG_BOOTCOMMAND					\
	"run nandboot; bootm ${loadaddr}"

/*
 * BOOTP options
 */
#define CONFIG_BOOTP_SUBNETMASK
#define CONFIG_BOOTP_GATEWAY
#define CONFIG_BOOTP_BOOTPATH
#define CONFIG_BOOTP_HOSTNAME
#define CONFIG_BOOTP_BOOTFILESIZE

#define CONFIG_SYS_TEXT_BASE 0x81080000

/* SPL */
#define CONFIG_SPL
#define CONFIG_SPL_TEXT_BASE 0x00000000
#define CONFIG_SPL_MAX_SIZE 0x8000

#define CONFIG_SPL_LIBCOMMON_SUPPORT
#define CONFIG_SPL_LIBGENERIC_SUPPORT
#define CONFIG_SPL_SERIAL_SUPPORT
#define CONFIG_SPL_LDSCRIPT "arch/arm/cpu/arm926ejs/lpc3250/u-boot-spl.lds"

#if 0
#define CONFIG_SPL_EARLY_DEBUG
#endif

#endif  /* __PTIP_H__*/
