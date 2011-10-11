/*
 * (C) Copyright 2002-2008
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <config.h>

/*
 * To build the utility with the run-time configuration
 * uncomment the next line.
 * See included "fw_env.config" sample file
 * for notes on configuration.
 */
#if 0
#define CONFIG_FILE     "/etc/fw_env.config"
#endif

#ifndef CONFIG_ENV_DEVICE1_NAME
#define CONFIG_ENV_DEVICE1_NAME "/dev/mtd1"
#endif
#ifndef CONFIG_ENV_DEVICE1_OFFSET
#define CONFIG_ENV_DEVICE1_OFFSET 0x0000
#endif
#ifndef CONFIG_ENV_DEVICE1_SIZE
#ifdef CONFIG_ENV_SIZE
#define CONFIG_ENV_DEVICE1_SIZE CONFIG_ENV_SIZE
#else
#define CONFIG_ENV_DEVICE1_SIZE 0x4000
#endif
#endif
#ifndef CONFIG_ENV_DEVICE1_ESIZE
#ifdef CONFIG_ENV_ERASE_SIZE
#define CONFIG_ENV_DEVICE1_ESIZE CONFIG_ENV_ERASE_SIZE
#else
#define CONFIG_ENV_DEVICE1_ESIZE 0x4000
#endif
#endif
#ifndef CONFIG_ENV_DEVICE1_SECTORS
#ifdef CONFIG_ENV_DEVICE_SECTORS
#define CONFIG_ENV_DEVICE1_SECTORS CONFIG_ENV_DEVICE_SECTORS
#elif defined(CONFIG_ENV_DEVICE1_SIZE) && defined(CONFIG_ENV_DEVICE1_ESIZE)
#define CONFIG_ENV_DEVICE1_SECTORS (CONFIG_ENV_DEVICE1_SIZE / CONFIG_ENV_DEVICE1_ESIZE)
#else
#define CONFIG_ENV_DEVICE1_SECTORS 1
#endif
#endif

#ifndef CONFIG_ENV_DEVICE2_NAME
#define CONFIG_ENV_DEVICE2_NAME "/dev/mtd2"
#endif
#ifndef CONFIG_ENV_DEVICE2_OFFSET
#define CONFIG_ENV_DEVICE2_OFFSET 0x0000
#endif
#ifndef CONFIG_ENV_DEVICE2_SIZE
#ifdef CONFIG_ENV_SIZE
#define CONFIG_ENV_DEVICE2_SIZE CONFIG_ENV_SIZE
#else
#define CONFIG_ENV_DEVICE2_SIZE 0x4000
#endif
#endif
#ifndef CONFIG_ENV_DEVICE2_ESIZE
#ifdef CONFIG_ENV_ERASE_SIZE
#define CONFIG_ENV_DEVICE2_ESIZE CONFIG_ENV_ERASE_SIZE
#else
#define CONFIG_ENV_DEVICE2_ESIZE 0x4000
#endif
#endif
#ifndef CONFIG_ENV_DEVICE2_SECTORS
#ifdef CONFIG_ENV_DEVICE_SECTORS
#define CONFIG_ENV_DEVICE2_SECTORS CONFIG_ENV_DEVICE_SECTORS
#elif defined(CONFIG_ENV_DEVICE2_SIZE) && defined(CONFIG_ENV_DEVICE2_ESIZE)
#define CONFIG_ENV_DEVICE2_SECTORS (CONFIG_ENV_DEVICE2_SIZE / CONFIG_ENV_DEVICE2_ESIZE)
#else
#define CONFIG_ENV_DEVICE2_SECTORS 1
#endif
#endif

#ifdef CONFIG_ENV_OFFSET_REDUND
#define HAVE_REDUND /* For systems with 2 env sectors */
#endif
#define DEVICE1_NAME CONFIG_ENV_DEVICE1_NAME
#define DEVICE2_NAME CONFIG_ENV_DEVICE2_NAME
#define DEVICE1_OFFSET CONFIG_ENV_DEVICE1_OFFSET
#define ENV1_SIZE CONFIG_ENV_DEVICE1_SIZE
#define DEVICE1_ESIZE CONFIG_ENV_DEVICE1_ESIZE
#define DEVICE1_ENVSECTORS CONFIG_ENV_DEVICE1_SECTORS
#define DEVICE2_OFFSET CONFIG_ENV_DEVICE2_OFFSET
#define ENV2_SIZE CONFIG_ENV_DEVICE2_SIZE
#define DEVICE2_ESIZE CONFIG_ENV_DEVICE2_ESIZE
#define DEVICE2_ENVSECTORS CONFIG_ENV_DEVICE2_SECTORS

extern int   fw_printenv(int argc, char *argv[]);
extern char *fw_getenv  (char *name);
extern int fw_setenv  (int argc, char *argv[]);
extern int fw_parse_script(char *fname);
extern int fw_env_open(void);
extern int fw_env_write(char *name, char *value);
extern int fw_env_close(void);

extern unsigned	long  crc32	 (unsigned long, const unsigned char *, unsigned);
