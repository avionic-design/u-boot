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

#include <asm/mach-types.h>

/*
 * Ethernet buffer support in uncached IRAM and buffer size
 */
#define USE_IRAM_FOR_ETH_BUFFERS
#define IRAM_ETH_BUFF_BASE 0x08010000 /* Uncached IRAM */
#define IRAM_ETH_BUFF_SIZE 0x00010000

/*
 * Linux machine type
 */
#define MACH_TYPE_UBOOTSYS CONFIG_PTIP_MACH_TYPE

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
#ifdef CONFIG_BOOTSTRAP_BUILD
#  define CONFIG_ENV_IS_NOWHERE
#  define CONFIG_ENV_SIZE	0x00008000 /* 2 blocks */
#  define CONFIG_ENV_ADDR	0x80000100 /* Passed to kernel here */
#else
#  define CONFIG_ENV_IS_IN_NAND	1
#  define CONFIG_ENV_SIZE	0x00008000 /* 2 blocks */
#  define CONFIG_ENV_OFFSET	0x00038000 /* Blocks 8/9  */
#  define CONFIG_ENV_ADDR	0x80000100 /* Passed to kernel here */
#endif

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
#ifdef CONFIG_BOOTSTRAP_BUILD
#  define CONFIG_BOOTDELAY -1
#else
#  define CONFIG_BOOTDELAY 3
#  define CONFIG_ZERO_BOOTDELAY_CHECK	/* check for keypress on bootdelay==0 */
#endif

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

#define CONFIG_CMD_MD5SUM
#ifdef CONFIG_CMD_MD5SUM
#  define CONFIG_MD5
#endif

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

#define LPC32XX_MLC_NAND_TIMING ( \
		MLC_TIMING_TCEA(3) | \
		MLC_TIMING_TWBTRB(15) | \
		MLC_TIMING_TRHZ(3) | \
		MLC_TIMING_TREH(7) | \
		MLC_TIMING_TRP(7) | \
		MLC_TIMING_TWH(7) | \
		MLC_TIMING_TWP(7))

/*
 * NAND H/W ECC specific settings
 */
#define CONFIG_SYS_LPC32XX_DMA            /* DMA support required */
#define CONFIG_SYS_NAND_PAGE_COUNT     32
#define CONFIG_SYS_NAND_PAGE_SIZE     512
#define CONFIG_SYS_NAND_BLOCK_SIZE    (CONFIG_SYS_NAND_PAGE_COUNT * CONFIG_SYS_NAND_PAGE_SIZE)
#define CONFIG_SYS_NAND_ECCSIZE       512 /* ECC generated per page */
#define CONFIG_SYS_NAND_ECCSTEPS      (CONFIG_SYS_NAND_PAGE_SIZE / CONFIG_SYS_NAND_ECCSIZE)
#define CONFIG_SYS_NAND_ECCBYTES        6 /* 3 Bytes ECC per 256 bytes */
#define CONFIG_SYS_NAND_ECCTOTAL      (CONFIG_SYS_NAND_ECCBYTES * CONFIG_SYS_NAND_ECCSTEPS)
#define CONFIG_SYS_NAND_OOBSIZE        16 /* OOB size in bytes */
#define CONFIG_SYS_NAND_4_ADDR_CYCLE    1 /* size > 32 MiB */

#define CONFIG_SYS_NAND_BAD_BLOCK_POS 5
#define CONFIG_SYS_NAND_ECCPOS { 10, 11, 12, 13, 14, 15 }

#define CONFIG_SYS_NAND_U_BOOT_DST 0x81080000
#define CONFIG_SYS_NAND_U_BOOT_START CONFIG_SYS_NAND_U_BOOT_DST
#define CONFIG_SYS_NAND_U_BOOT_OFFS (256 * 1024)
#define CONFIG_SYS_NAND_U_BOOT_SIZE (512 * 1024)

#ifdef CONFIG_CMD_MTDPARTS
#  define CONFIG_MTD_DEVICE	1
#  define CONFIG_MTD_PARTITIONS	1

#  define MTDIDS_DEFAULT "nand0=flash"

#if CONFIG_PTIP_MACH_TYPE == MACH_TYPE_PTIP_MURNAU
#  define MTDPARTS_DEFAULT "mtdparts=flash:"				\
	"224k(kickstart),32k(environment),512k(uboot),"			\
	"8m(uimage),"							\
	"23168k(rootfs1),23168k(rootfs2),-(persist)"
#else
#  define MTDPARTS_DEFAULT "mtdparts=flash:"				\
	"224k(kickstart),32k(environment),512k(uboot),"			\
	"4m(uimage1),4m(uimage2),"					\
	"23168k(rootfs1),23168k(rootfs2),-(persist)"
#endif

#endif

#if defined(CONFIG_CMD_UBI) || defined(CONFIG_CMD_UBIFS)
#  define CONFIG_LZO		1
#  define CONFIG_RBTREE		1
#endif

/*
 * Support for various capabilities
 */
#define CONFIG_AUTO_COMPLETE
#define CONFIG_CMDLINE_EDITING
#define CONFIG_SYS_LOADS_BAUD_CHANGE

/*
 * Network setup
 */
#define CONFIG_LPC32XX_ETH	1
#define CONFIG_NET_MULTI	1

#ifdef CONFIG_LPC32XX_ETH
#define CONFIG_NETMASK		255.255.255.0
#define CONFIG_IPADDR		192.168.1.101
#define CONFIG_SERVERIP		192.168.1.41
#define CONFIG_ETHADDR		00:1a:f1:00:00:00
#define CONFIG_GATEWAYIP	192.168.1.1

#define CONFIG_BOOTFILE		uImage
#define CONFIG_LOADADDR		0x80800000
#define CONFIG_ROOTPATH		/home/user/ltib/rootfs

/*
 * BOOTP options
 */
#define CONFIG_BOOTP_SUBNETMASK
#define CONFIG_BOOTP_GATEWAY
#define CONFIG_BOOTP_BOOTPATH
#define CONFIG_BOOTP_HOSTNAME
#define CONFIG_BOOTP_BOOTFILESIZE
#endif

#define ADD_ETHADDR "add-ethaddr="				\
	"set bootargs ${bootargs} ethaddr=${ethaddr}\0"

#define ADD_MTD_ROOT "add-mtd-root="				\
	"set bootargs ${bootargs} root=${mtdroot} rootfstype=${mtdfstype}\0"

#define ADD_NFS_ROOT "add-nfs-root="				\
	"set bootargs ${bootargs} root=/dev/nfs nfsroot=${serverip}:${rootpath} ip=dhcp\0"

#define COMMON_BOOTARGS "common-bootargs="			\
	"console=ttyS0,115200n8 init=/init ro\0"

#define SET_BOOTARGS "set-bootargs="				\
	"set bootargs ${common-bootargs} ; run add-ethaddr add-${roottype}-root\0"

#define BOOT_CONFIG						\
	"mtdroot=/dev/mtdblock4\0"				\
	"mtdfstype=squashfs\0"					\
	"boottype=nand\0"					\
	"roottype=mtd\0"					\
	""

#define BOOT_NAND "nandboot="					\
	"nand read ${loadaddr} 0x000c0000 0x00800000\0"

#define UPDATE_UBOOT "update_uboot="				\
	"nand erase 0x00040000 0x00080000;"			\
	"nand write ${loadaddr} 0x00040000 0x00080000;\0"

#if CONFIG_PTIP_MACH_TYPE == MACH_TYPE_PTIP_MURNAU
#define UPDATE_UIMAGE "update_uimage="				\
	"nand erase 0x000c0000 0x00800000;"			\
	"nand write ${loadaddr} 0x000c0000 0x00800000;\0"
#else
#define UPDATE_UIMAGE "update_uimage1="				\
	"nand erase 0x000c0000 0x00400000;"			\
	"nand write ${loadaddr} 0x000c0000 0x00400000;\0"	\
		"update_uimage2="				\
	"nand erase 0x004c0000 0x00400000;"			\
	"nand write ${loadaddr} 0x004c0000 0x00400000;\0"
#endif

#define UPDATE_ROOTFS1 "update_rootfs1="			\
	"nand erase 0x008c0000 0x016a0000;"			\
	"nand write ${loadaddr} 0x008c0000 0x016a0000;\0"

#define UPDATE_ROOTFS2 "update_rootfs2="			\
	"nand erase 0x01f60000 0x016a0000;"			\
	"nand write ${loadaddr} 0x01f60000 0x016a0000;\0"

/*
 * Other preset environment variables and example bootargs string
 */
#define CONFIG_EXTRA_ENV_SETTINGS				\
	ADD_ETHADDR						\
	ADD_MTD_ROOT						\
	ADD_NFS_ROOT						\
	COMMON_BOOTARGS						\
	SET_BOOTARGS						\
	BOOT_CONFIG						\
	BOOT_NAND						\
	UPDATE_UBOOT						\
	UPDATE_UIMAGE						\
	UPDATE_ROOTFS1						\
	UPDATE_ROOTFS2						\

/* Default boot command */
#define CONFIG_BOOTCOMMAND					\
	"run set-bootargs ${boottype}boot; bootm ${loadaddr}"

#endif  /* __PTIP_H__*/
