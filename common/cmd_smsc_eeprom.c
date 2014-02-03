#include <common.h>
#include <command.h>
#include <usb.h>
#include "usb_ether.h"

#define SMSC_SIGNATURE 0xA5
#define SMSC_EEPROM_OFFSET_MAC 1

static u8 eeprom_defaults[] = {
	/* 0x00 */
	0xA5,		/* Signature */
	0xFF, 0xFF,	/* MAC bytes 0-1 */
	0xFF, 0xFF,	/* MAC bytes 2-3 */
	0xFF, 0xFF,	/* MAC bytes 4-5 */
	0x01,		/* FS Polling Interval for Interrupt Endpoint */
	0x01,		/* HS Polling Interval for Interrupt Endpoint */
	0x01,		/* Configuration Flags */
	0x09, 0x04,	/* Language ID */
	0x0a,		/* Manufacturer ID String Descriptor Length (bytes) */
	0x2f,		/* Manufacturer ID String Descriptor EEPROM Word Offset */
	0x10,		/* Product Name String Descriptor Length (bytes) */
	0x34,		/* Product Name String Descriptor EEPROM Word Offset */
	/* 0x10 */
	0x12,		/* Serial Number String Descriptor Length (bytes) */
	0x3c,		/* Serial Number String Descriptor EEPROM Word Offset */
	0x08,		/* Configuration String Descriptor Length (bytes) */
	0x45,		/* Configuration String Descriptor Word Offset */
	0x08,		/* Interface String Descriptor Length (bytes) */
	0x49,		/* Interface String Descriptor Word Offset */
	0x12,		/* Hi-Speed Device Descriptor Length (bytes) */
	0x1d,		/* Hi-Speed Device Descriptor Word Offset */
	0x12,		/* Hi-Speed Configuration and Interface Descriptor Length (bytes) */
	0x26,		/* Hi-Speed Configuration and Interface Descriptor Word Offset */
	0x12,		/* Full-Speed Device Descriptor Length (bytes) */
	0x1d,		/* Full-Speed Device Descriptor Word Offset */
	0x12,		/* Full-Speed Configuration and Interface Descriptor Length (bytes) */
	0x26,		/* Full-Speed Configuration and Interface Descriptor Word Offset */
	0x00, 0x00,	/* RESERVED */
	/* 0x20 */
	0x24, 0x04,	/* Vendor ID */
	0x14, 0x95,	/* Product ID */
	0x00, 0x01,	/* Device ID */
	0x9b,		/* Config Data Byte 1 Register (CFG1) */
	0x18,		/* Config Data Byte 2 Register (CFG2) */
	0x00,		/* Config Data Byte 3 Register (CFG3) */
	0x32,		/* Non-Removable Devices Register (NRD) */
	0x00,		/* Port Disable (Self) Register (PDS) */
	0x00,		/* Port Disable (Bus) Register (PDB) */
	0x01,		/* Max Power (Self) Register (MAXPS) */
	0x00,		/* Max Power (Bus) Register (MAXPB) */
	0x01,		/* Hub Controller Max Current (Self) Register (HCMCS) */
	0x00,		/* Hub Controller Max Current (Bus) Register (HCMCB) */
	/* 0x30 */
	0x32,		/* Power-on Time Register (PWRT) */
	0x00,		/* Boost_Up Register (BOOSTUP) */
	0x00,		/* Boost_5 Register (BOOST5) */
	0x00,		/* Boost_4:2 Register (BOOST42) */
	0x00,		/* RESERVED */
	0x00,		/* Port Swap Register (PRTSP) */
	0x21,		/* Port Remap 12 Register (PRTR12) */
	0x43,		/* Port Remap 34 Register (PRTR34) */
	0x05,		/* Port Remap 5 Register (PRTR5) */
	0x01,		/* Status/Command Register (STCD) */
	/* 0x3A		 - Device Descriptor */
	0x12, 0x01,
	0x00, 0x02,
	0xff, 0x00,
	/* 0x40 */
	0xff, 0x40,
	0x24, 0x04,
	0x00, 0xec,
	0x00, 0x01,
	0x01, 0x02,
	0x03, 0x01,
	/* 0x4C		 - Configuration and Interface Descriptor */
	0x09, 0x02,
	0x27, 0x00,
	/* 0x50 */
	0x01, 0x01,
	0x04, 0xc0,
	0x00, 0x09,
	0x04, 0x00,
	0x00, 0x03,
	0xff, 0x00,
	0xff, 0x05,
	/* 0x5E		 - Manufacturer ID String Descriptor */
	0x0a, 0x03,
	/* 0x60 */
	0x53, 0x00,	/* S */
	0x4d, 0x00,	/* M */
	0x53, 0x00,	/* S */
	0x43, 0x00,	/* C */
	/* 0x68		 - Product Name String */
	0x10, 0x03,
	0x4c, 0x00,	/* L */
	0x41, 0x00,	/* A */
	0x4e, 0x00,	/* N */
	/* 0x70 */
	0x39, 0x00,	/* 9 */
	0x35, 0x00,	/* 5 */
	0x31, 0x00,	/* 1 */
	0x34, 0x00,	/* 5 */
	/* 0x78		 - Serial Number String Descriptor */
	0x12, 0x03,
	0x31, 0x00,	/* 1 */
	0x32, 0x00,	/* 2 */
	0x33, 0x00,	/* 3 */
	/* 0x80 */
	0x34, 0x00,	/* 4 */
	0x35, 0x00,	/* 5 */
	0x36, 0x00,	/* 6 */
	0x37, 0x00,	/* 7 */
	0x38, 0x00,	/* 8 */
	/* 0x8A		 - Configuration String Descriptor */
	0x08, 0x03,
	0x43, 0x00,	/* C */
	0x66, 0x00,	/* f */
	/* 0x90 */
	0x67, 0x00,	/* g */
	/* 0x92		 - Interface String Descriptor */
	0x08, 0x03,
	0x69, 0x00,	/* i */
	0x2f, 0x00,	/* / */
	0x66, 0x00,	/* f */
	/* 0x9A - END */
	0x00, 0x00,
	0x00, 0x00,
	0x00, 0x00,
	/* 0xA0 */
};

static int do_smsc_eeprom_rw(struct ueth_data *usb_eth,
			int argc, char * const argv[])
{
	ulong addr, offset, length = 1;

	if (argc < 4)
		return CMD_RET_USAGE;

	addr = simple_strtoul(argv[2], NULL, 16);
	offset = simple_strtoul(argv[3], NULL, 16);
	if (argc > 4)
		length = simple_strtoul(argv[4], NULL, 16);

	if (!strcmp(argv[0], "write")) {
		if (smsc95xx_write_eeprom(usb_eth, offset,
						length, (void*)addr)) {
			printf("EEPROM write failed\n");
			return CMD_RET_FAILURE;
		}
	} else if (!strcmp(argv[0], "read")) {
		if (smsc95xx_read_eeprom(usb_eth, offset,
						length, (void*)addr)) {
			printf("EEPROM read failed\n");
			return CMD_RET_FAILURE;
		}
	} else
		return CMD_RET_USAGE;

	return CMD_RET_SUCCESS;
}

static int do_smsc_eeprom_defaults(struct ueth_data *usb_eth,
				int argc, char * const argv[])
{
	if (smsc95xx_write_eeprom(usb_eth, 0, sizeof(eeprom_defaults),
					eeprom_defaults)) {
		printf("EEPROM write failed\n");
		return CMD_RET_FAILURE;
	}

	return CMD_RET_SUCCESS;
}

static int do_smsc_eeprom_set_mac(struct ueth_data *usb_eth,
				int argc, char * const argv[])
{
	u8 mac[6];

	if (argc < 3)
		return CMD_RET_USAGE;

	eth_parse_enetaddr(argv[2], mac);
	if (!is_valid_ether_addr(mac)) {
		printf("Invalid mac address given\n");
		return CMD_RET_FAILURE;
	}

	printf("Writing smsc95xx MAC to EEPROM ....\n");
	if (smsc95xx_write_eeprom(usb_eth, SMSC_EEPROM_OFFSET_MAC,
					sizeof(mac), mac)) {
		printf("EEPROM write failed\n");
		return CMD_RET_FAILURE;
	}

	return CMD_RET_SUCCESS;
}

static int do_smsc_eeprom_show_mac(struct ueth_data *usb_eth,
				int argc, char * const argv[])
{
	u8 data[7];

	if (smsc95xx_read_eeprom(usb_eth, 0, sizeof(data), data)) {
		printf("EEPROM read failed\n");
		return CMD_RET_FAILURE;
	}

	if (data[0] != SMSC_SIGNATURE)
		printf("Warning: EEPROM signature is invalid (0x%02x)\n",
			data[0]);

	printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
		data[1], data[2], data[3], data[4], data[5], data[6]);

	if (!is_valid_ether_addr(&data[1]))
		printf("Warning: MAC address is not valid!\n");

	return CMD_RET_SUCCESS;
}

static int do_smsc_eeprom(cmd_tbl_t *cmdtp, int flag, int argc,
			char * const argv[])
{
	struct ueth_data *usb_eth;
	int eth_index;

	if (argc < 3)
		return CMD_RET_USAGE;

	eth_index = (int) simple_strtoul(argv[2], NULL, 16);
	usb_eth = get_usb_eth(eth_index);
	if (strncmp("sms", usb_eth->eth_dev.name, 3)) {
		printf("Enter a valid smsc eth index\n");
		return -1;
	}

	if (!strcmp(argv[1], "read") || !strcmp(argv[1], "write"))
		return do_smsc_eeprom_rw(usb_eth, argc - 1, argv + 1);
	if (!strcmp(argv[1], "defaults"))
		return do_smsc_eeprom_defaults(usb_eth, argc - 1, argv + 1);
	if (!strcmp(argv[1], "set_mac"))
		return do_smsc_eeprom_set_mac(usb_eth, argc - 1, argv + 1);
	if (!strcmp(argv[1], "show_mac"))
		return do_smsc_eeprom_show_mac(usb_eth, argc - 1, argv + 1);

	printf("Unknown sub command: %s\n", argv[1]);

	return CMD_RET_USAGE;
}

U_BOOT_CMD(
	smsc_eeprom,	6,	0,	do_smsc_eeprom,
	"SMSC EEPROM",
	"smsc_eeprom read - eth-index addr off [size]\n"
	"smsc_eeprom write - eth-index addr off [size]\n"
	"    read/write 'size' bytes starting at offset 'off'\n"
	"    to/from memory address 'addr'.\n"
	"smsc_eeprom defaults - eth-index\n"
	"    write default settings in the EEPROM.\n"
	"smsc_eeprom set_mac - eth-index mac\n"
	"    set the MAC address in the EEPROM to 'mac'\n"
	"smsc_eeprom show_mac - eth-index\n"
	"    read the MAC address from the EEPROM."
);
