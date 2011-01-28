/*
 * Copyright (C) 2009 Texas Instruments Incorporated
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/* Avionic Design Cabin Surveillance Camera board */
#define CSC

#define CONFIG_SKIP_LOWLEVEL_INIT	/* U-Boot is a 3rd stage loader */
#define CONFIG_SKIP_RELOCATE_UBOOT
#define CONFIG_SYS_NO_FLASH		/* that is, no *NOR* flash */
#define CONFIG_SYS_CONSOLE_INFO_QUIET

/* SoC Configuration */
#define CONFIG_ARM926EJS				/* arm926ejs CPU */
#define CONFIG_SYS_TIMERBASE		0x01c21400	/* use timer 0 */
#define CONFIG_SYS_HZ_CLOCK		24000000	/* timer0 freq */
#define CONFIG_SYS_HZ			1000
#define CONFIG_SOC_DM365

/* Memory Info */
#define CONFIG_NR_DRAM_BANKS		1
#define PHYS_SDRAM_1			0x80000000
#define PHYS_SDRAM_1_SIZE		(256 << 20)	/* 256 MiB */

/* Serial Driver info: UART0 for console  */
#define CONFIG_SYS_NS16550
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_SYS_NS16550_REG_SIZE	-4
#define CONFIG_SYS_NS16550_COM1		0x01c20000
#define CONFIG_SYS_NS16550_CLK		CONFIG_SYS_HZ_CLOCK
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }
#define CONFIG_CONS_INDEX		1
#define CONFIG_BAUDRATE			115200

/* EEPROM definitions for EEPROM on DM365 EVM */
#define CONFIG_SYS_I2C_EEPROM_ADDR_LEN		2
#define CONFIG_SYS_I2C_EEPROM_ADDR		0x50
#define CONFIG_SYS_EEPROM_PAGE_WRITE_BITS	6
#define CONFIG_SYS_EEPROM_PAGE_WRITE_DELAY_MS	20

/* Network Configuration */
#define CONFIG_DRIVER_TI_EMAC
#define CONFIG_EMAC_MDIO_PHY_NUM	5
#define CONFIG_MII
#define CONFIG_BOOTP_DEFAULT
#define CONFIG_BOOTP_DNS
#define CONFIG_BOOTP_DNS2
#define CONFIG_BOOTP_SEND_HOSTNAME
#define CONFIG_NET_RETRY_COUNT	10
#define CONFIG_NET_MULTI

/* I2C */
#define CONFIG_HARD_I2C
#define CONFIG_DRIVER_DAVINCI_I2C
#define CONFIG_SYS_I2C_SPEED		400000
#define CONFIG_SYS_I2C_SLAVE		0x10	/* SMBus host address */

/* NAND */
#define CONFIG_NAND_DAVINCI
#define CONFIG_SYS_NAND_CS		2
#define CONFIG_SYS_NAND_USE_FLASH_BBT
#define CONFIG_SYS_NAND_4BIT_HW_ECC_OOBFIRST
#define CONFIG_SYS_NAND_PAGE_2K
#undef  CONFIG_SYS_NAND_RBL_OOB_LAYOUT

#define CONFIG_SYS_NAND_LARGEPAGE
#define CONFIG_SYS_NAND_BASE_LIST	{ 0x02000000, }
/* socket has two chipselects, nCE0 gated by address BIT(14) */
#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define CONFIG_SYS_NAND_MAX_CHIPS	1

#define PINMUX4_USBDRVBUS_BITCLEAR       0x3000
#define PINMUX4_USBDRVBUS_BITSET         0x2000

/* USB Configuration */
#define CONFIG_USB_DAVINCI
#define CONFIG_MUSB_HCD

#ifdef CONFIG_USB_DAVINCI
#define CONFIG_CMD_USB         /* include support for usb      */
#define CONFIG_CMD_STORAGE     /* include support for usb      */
#define CONFIG_CMD_FAT         /* include support for FAT/storage*/
#define CONFIG_DOS_PARTITION   /* include support for FAT/storage*/
#endif

#ifdef CONFIG_MUSB_HCD         /* include support for usb host */
#define CONFIG_CMD_USB         /* include support for usb cmd */
#define CONFIG_USB_STORAGE     /* MSC class support */
#define CONFIG_CMD_STORAGE     /* inclue support for usb-storage cmd */
#define CONFIG_CMD_FAT         /* inclue support for FAT/storage */
#define CONFIG_DOS_PARTITION   /* inclue support for FAT/storage */

#ifdef CONFIG_USB_KEYBOARD     /* HID class support */
#define CONFIG_SYS_USB_EVENT_POLL

#define CONFIG_PREBOOT "usb start"
#endif /* CONFIG_USB_KEYBOARD */
#endif /* CONFIG_MUSB_HCD */

#ifdef CONFIG_MUSB_UDC
#define CONFIG_USB_DEVICE              1
#define CONFIG_USB_TTY                 1
#define CONFIG_SYS_CONSOLE_IS_IN_ENV   1
#define CONFIG_USBD_VENDORID           0x0451
#define CONFIG_USBD_PRODUCTID          0x5678
#define CONFIG_USBD_MANUFACTURER       "Texas Instruments"
#define CONFIG_USBD_PRODUCT_NAME       "DM365VM"
#endif /* CONFIG_MUSB_UDC */

/* U-Boot command configuration */
#include <config_cmd_default.h>

#undef CONFIG_CMD_BDI
#undef CONFIG_CMD_FLASH
#undef CONFIG_CMD_FPGA
#undef CONFIG_CMD_SETGETDCR

#define CONFIG_CMD_ASKENV
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_I2C
#define CONFIG_CMD_PING
#define CONFIG_CMD_SAVES

#ifdef CONFIG_NAND_DAVINCI
#define CONFIG_CMD_MTDPARTS
#define CONFIG_MTD_PARTITIONS
#define CONFIG_MTD_DEVICE
#define CONFIG_CMD_NAND
#define CONFIG_CMD_UBI
#define CONFIG_RBTREE
#endif

#ifdef CONFIG_DRIVER_TI_EMAC
#define CONFIG_CMD_MII
#endif

#define CONFIG_CRC32_VERIFY
#define CONFIG_MX_CYCLIC

/* U-Boot general configuration */
#undef CONFIG_USE_IRQ				/* No IRQ/FIQ in U-Boot */
#define CONFIG_SYS_PROMPT	"CSC # "	/* Monitor Command Prompt */
#define CONFIG_SYS_CBSIZE	1024		/* Console I/O Buffer Size  */
#define CONFIG_SYS_PBSIZE			/* Print buffer size */ \
		(CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_MAXARGS	16		/* max number of command args */
#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_PROMPT_HUSH_PS2	"> "
#define CONFIG_SYS_LONGHELP

#ifdef CONFIG_NAND_DAVINCI
#define CONFIG_ENV_SIZE			(256 << 10)	/* 256 KiB */
#define CONFIG_ENV_IS_IN_NAND
#define CONFIG_ENV_OFFSET		0x640000
#define CONFIG_ENV_OFFSET_REDUND	0x720000
#undef CONFIG_ENV_IS_IN_FLASH

#define CONFIG_ENV_ERASE_SIZE		0x020000
#define CONFIG_ENV_DEVICE_SECTORS	((CONFIG_ENV_OFFSET_REDUND - CONFIG_ENV_OFFSET) / CONFIG_ENV_ERASE_SIZE)

#define CONFIG_ENV_DEVICE1_NAME		"/dev/mtd2"
#define CONFIG_ENV_DEVICE1_OFFSET	0x000000
#define CONFIG_ENV_DEVICE2_NAME		CONFIG_ENV_DEVICE1_NAME
#define CONFIG_ENV_DEVICE2_OFFSET	(CONFIG_ENV_DEVICE_SECTORS * CONFIG_ENV_ERASE_SIZE)
#endif

#define CONFIG_BOOTDELAY	10
#define CONFIG_BOOTCOMMAND	"run apply-updates loadk set-bootargs ; bootm"
#define CONFIG_BOOTARGS		""

#define CONFIG_CMDLINE_EDITING
#define CONFIG_VERSION_VARIABLE
#define CONFIG_TIMESTAMP

/* U-Boot memory configuration */
#define CONFIG_STACKSIZE		(256 << 10)	/* 256 KiB */
#define CONFIG_SYS_MALLOC_LEN		(1 << 20)	/* 1 MiB */
#define CONFIG_SYS_GBL_DATA_SIZE	128		/* for initial data */
#define CONFIG_SYS_MEMTEST_START	0x87000000	/* physical address */
#define CONFIG_SYS_MEMTEST_END		0x88000000	/* test 16MB RAM */

/* Linux interfacing */
#define CONFIG_CMDLINE_TAG
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_SYS_BARGSIZE	1024			/* bootarg Size */
#define CONFIG_SYS_LOAD_ADDR	0x80700000		/* kernel address */

/* NAND layout */
#define MTDIDS_DEFAULT		"nand0=davinci_nand.0"

#define PART_UBL		"3200k(ubl)ro,"
#define PART_UBOOT		"3200k(uboot),"
#define PART_PARAMS		"1792k(params),"
#define PARTS_BOOT		PART_UBL PART_UBOOT PART_PARAMS

#define PART_KERNEL_1		"4m(kernel1),"
#define PART_KERNEL_2		"4m(kernel2),"
#define PARTS_KERNEL		PART_KERNEL_1 PART_KERNEL_2

#define PART_ROOT_1		"192m(root1),"
#define PART_ROOT_2		"192m(root2),"
#define PART_LOGS		"-(logs)"
#define PARTS_FS		PART_ROOT_1 PART_ROOT_2 PART_LOGS

#define MTDPARTS_DEFAULT	\
	"mtdparts=davinci_nand.0:" PARTS_BOOT PARTS_KERNEL PARTS_FS

#define CONFIG_MAX_RAM_BANK_SIZE       (256 << 20)     /* 256 MB */

#define CONFIG_SYS_SDRAM_BASE          PHYS_SDRAM_1
#define CONFIG_SYS_INIT_SP_ADDR                \
	(CONFIG_SYS_SDRAM_BASE + 0x1000 - GENERATED_GBL_DATA_SIZE)

/* Default kernel loading method: dhcp, tftp, nand1 or nand2 */
#define BOOT_LOADK_DEFAULT_MODE		"nand1"
/* Default root mode: nfs or nand */
#define BOOT_ROOT_DEFAULT_MODE		"nfs"

/* Default root partition when running from NAND */
#define NAND_BOOT_DEFAULT_DEV		"/dev/mtdblock5"
#define NFS_BOOT_DEFAULT_SERVERIP	"172.21.30.139"
#define NFS_BOOT_DEFAULT_ROOTPATH	"/home/alban/roots/pbs_dm365"

/* Default kernel parameters needed for ALL configs */
#define BASE_KERNEL_ARGS		"console=ttyS0,115200n8 vpfe-capture.interface=1"
/* Memory related parameters */
#define MEM_KERNEL_ARGS			"mem=188M cmemk.phys_start=0x8BC00000 cmemk.phys_end=0x90000000"

/* Defaults when running from NFS */
#define CONFIG_BOOTFILE			"dm365/uImage"	/* Boot file name */
#define CONFIG_SERVERIP			"172.21.30.139"
#define CONFIG_ROOTPATH			"/home/alban/roots/pbs_dm365"


/* Setup the boot command */
#define BOOT_ENV \
    "baseargs="BASE_KERNEL_ARGS"\0"					\
    "memargs="MEM_KERNEL_ARGS"\0"					\
    "kernel-from="BOOT_LOADK_DEFAULT_MODE"\0"				\
    "boot-from="BOOT_ROOT_DEFAULT_MODE"\0"				\
    "set-bootargs=run set-${boot-from}-args ;"				\
                " setenv bootargs ${memargs} ${baseargs} "		\
                                 "${rootargs} ${mtdparts} "		\
                                 "u-boot.kernel-from=${kfound}\0"	\

/* Setup rootargs to run from NAND */
#define NAND_BOOT_ENV \
    "set-nand-args=setenv rootargs root=${nand-dev} ${nand-mode}"	\
                          " rootfstype=jffs2\0"				\
    "nand-mode=ro\0"							\
    "nand-dev="NAND_BOOT_DEFAULT_DEV"\0"				\

/* Setup rootargs to run from NFS */
#define NFS_BOOT_ENV \
    "set-nfs-args=setenv rootargs root=/dev/nfs ${nfs-mode}"		\
                         " nfsroot=${serverip}:${rootpath} ip=dhcp\0"	\
    "nfs-mode=ro\0"							\
    "serverip="NFS_BOOT_DEFAULT_SERVERIP"\0"				\
    "rootpath="NFS_BOOT_DEFAULT_ROOTPATH"\0"				\

/* Various method to load a kernel in memory */
#define LOAD_KERNEL_ENV \
    "loadk=kfound=; "							\
        "for kfrom in ${kernel-from} ; do "				\
            "if test \"x$kfound\" = \"x\" ; then "			\
                "run loadk-$kfrom && kfound=$kfrom ; "			\
            "fi ; "							\
        "done ; "							\
        "if test \"x$kfound\" = \"x\" ; then "				\
            "echo Failed to load kernel! ; "				\
            "reset ; "							\
        "fi ; "								\
        "true\0"							\
    "loadk-nand1=nboot nand0,3\0"					\
    "loadk-nand2=nboot nand0,4\0"					\
    "loadk-tftp=tftpboot\0"						\
    "loadk-dhcp=dhcp\0"							\

/* MTD stuff */
#define MTD_ENV \
    "mtdids="MTDIDS_DEFAULT"\0"						\
    "mtdparts="MTDPARTS_DEFAULT"\0"					\

/* Update stuff */
#define UPDATE_ENV \
    "updateable-vars="							\
        "boot-from kernel-from "					\
        "nand-dev "							\
        "serverip rootpath bootfile\0"					\
    "get-next-value="							\
        "next_var=next-$var ; "						\
        "setenv -set-next-value next_value=\\$\\{$next_var\\} ; "	\
        "run -set-next-value ; "					\
        "setenv -set-next-value\0"					\
    "apply-updates="							\
       "setenv -apply-var-updates ; "					\
       "for var in ${updateable-vars} ; do "				\
           "run get-next-value ; "					\
           "if test \"x$next_value\" != \"x\" ; then ; "		\
               "setenv ${next_var} ; "					\
               "setenv -apply-var-updates ${-apply-var-updates} \\; "	\
                                     "setenv ${var} ${next_value} ; "	\
           "fi ; "							\
       "done ; "							\
       "if test \"x${-apply-var-updates}\" != \"x\" ; then ; "		\
           "saveenv ; saveenv ; "					\
           "run -apply-var-updates ; "					\
           "setenv -apply-var-updates ; "				\
       "fi ; "								\
       "true"								\

/* All the additional environement we need */
#define CONFIG_EXTRA_ENV_SETTINGS \
    BOOT_ENV			  \
    NAND_BOOT_ENV		  \
    NFS_BOOT_ENV		  \
    LOAD_KERNEL_ENV		  \
    MTD_ENV			  \
    UPDATE_ENV			  \

#endif /* __CONFIG_H */
