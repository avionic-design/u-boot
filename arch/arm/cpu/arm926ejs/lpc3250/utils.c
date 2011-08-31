static void serial_put_x8(u8 value)
{
	char buffer[3];
	int i;

	buffer[2] = '\0';

	for (i = 0; i < 2; i++) {
		if ((value & 0xf) >= 10)
			buffer[1 - i] = 'a' + ((value & 0xf) - 10);
		else
			buffer[1 - i] = '0' + (value & 0xf);

		value >>= 4;
	}

	serial_puts(buffer);
}

static void serial_put_x32(u32 value)
{
	char buffer[9];
	int i;

	buffer[8] = '\0';

	for (i = 0; i < 8; i++) {
		if ((value & 0xf) >= 10)
			buffer[7 - i] = 'a' + ((value & 0xf) - 10);
		else
			buffer[7 - i] = '0' + (value & 0xf);

		value >>= 4;
	}

	serial_puts(buffer);
}

static void serial_hexdump(const void *buffer, size_t size)
{
	const u8 *ptr = (u8 *)buffer;
	unsigned int i, j;

	serial_puts("dumping data @");
	serial_put_x32((u32)buffer);
	serial_puts("\r\n");

	for (i = 0; i < size; i += 16) {
		serial_puts("  ");

		for (j = 0; j < 16; j++) {
			serial_put_x8(*ptr++);
			serial_puts(" ");
		}

		serial_puts("\r\n");
	}
}
