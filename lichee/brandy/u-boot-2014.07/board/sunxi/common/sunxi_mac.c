#include <common.h>
#include <smc.h>
#include <u-boot/md5.h>
#include <fdt_support.h>
#include <sunxi_board.h>
#include <sys_partition.h>

#define pr_msg   tick_printf
#define pr_error tick_printf

static int get_macaddr_from_secure(char* mac)
{
	char *p = NULL;
	/* mac like 10:11:12:13:14:15 size 17 */
	p = getenv("androidboot.mac");
	if (p != NULL) {
		pr_msg("%s, mac = %s\n", __func__, p);
		if (strlen(p) != 17) {
			return -1;
		}
		if (!strncasecmp(p + 4, "0:00:00:00:00", 13)
		 || !strncasecmp(p + 4, "F:FF:FF:FF:FF", 13)) {
			return -1;
		}
		strcpy(mac, p);
		return 0;
	}

	return -1;
}

static int get_macaddr_from_file(char* mac)
{
	int  nodeoffset;
	int  ret = 0;
	int  partno = -1;
	char *filename = NULL;
	char part_info[16] = {0};  /* format: "partno:0" */
	char addr_info[32] = {0};  /* 00000000           */
	char file_info[64] = {0};

	/* 0x11 means 17 bytes to read, e.g. 00:11:22:33:44:55 */
	char *bmp_argv[6] = { "fatload", "sunxi_flash", part_info, addr_info, file_info, "11" };

	/* check serial_feature config info */
	nodeoffset = fdt_path_offset(working_fdt, "/soc/serial_feature");
	if (nodeoffset < 0) {
		pr_error("serial_feature is not exist\n");
		return -1;
	}

	ret = fdt_getprop_string(working_fdt, nodeoffset, "mac_filename", &filename);
	if((ret < 0) || (strlen(filename)== 0) ) {
		pr_msg("mac_filename is not exist, use default: ULI/factory/mac.txt\n");
		sprintf(file_info, "%s", "ULI/factory/mac.txt");
	}

	/* check private partition info */
	partno = sunxi_partition_get_partno_byname("private");
	if(partno < 0) {
		return -1;
	}

	/* get data from file */
	sprintf(part_info, "%x:0", partno);
	sprintf(addr_info, "%lx", (ulong)mac);
	if(do_fat_fsload(0, 0, 6, bmp_argv)) {
		pr_error("load file(%s) error, try mac.txt\n", bmp_argv[4]);
		sprintf(file_info, "%s", "mac.txt");
		if(do_fat_fsload(0, 0, 6, bmp_argv)) {
			pr_error("load file(%s) error, no more retry\n", bmp_argv[4]);
			return -1;
		}
	}

	pr_msg("%s, mac = %s\n", __func__, mac);
	if (strlen(mac) != 17) {
		return -1;
	}

	if (!strncasecmp(mac + 4, "0:00:00:00:00", 13)
	 || !strncasecmp(mac + 4, "F:FF:FF:FF:FF", 13)) {
		return -1;
	}

	return 0;
}

static int get_macaddr_from_chipid(char* mac)
{
#ifndef CONFIG_MD5
	pr_error("Caution: %s dependents on CONFIG_MD5, please check your config!\n", __func__);
	return -1;
#else
	u32 sid[4];
	u8  buf[16];

	sid[0] = smc_readl(SUNXI_SID_BASE + 0x200);
	sid[1] = smc_readl(SUNXI_SID_BASE + 0x200 + 0x4);
	sid[2] = smc_readl(SUNXI_SID_BASE + 0x200 + 0x8);
	sid[3] = smc_readl(SUNXI_SID_BASE + 0x200 + 0xc);

	pr_msg("SID: %08X-%08X-%08X-%08X\n", sid[0], sid[1], sid[2], sid[3]);
	if((sid[0] == 0) && (sid[1] == 0) && (sid[2] == 0) && (sid[3] == 0x0))
		return -1;

	md5_wd((unsigned char *)sid, 16, buf, CHUNKSZ_MD5);
	sprintf(mac, "%02x:%02x:%02x:%02x:%02x:%02x", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);
	if (!strncasecmp(mac + 4, "0:00:00:00:00", 13)
	 || !strncasecmp(mac + 4, "F:FF:FF:FF:FF", 13)) {
		return -1;
	}
	return 0;
#endif
}

int update_sunxi_mac(void)
{
	char mac[18];
	int ret;

	/* bits:   47:44    43:42  41     40        39:36  35:0 */
	/* format: platform macsrc global broadcast port   mac  */
	ret = get_macaddr_from_secure(mac);
	if (ret != 0) {
		pr_error("Caution: mac from storge fail!\n");
	} else {
		mac[1] = '0'; /* (0 << 2), mac from secure storge */
		goto out;
	}

	ret = get_macaddr_from_file(mac);
	if (ret != 0) {
		pr_error("Caution: mac from file fail!\n");
	} else {
		mac[1] = '4'; /* (1 << 2), mac from file */
		goto out;
	}

	ret = get_macaddr_from_chipid(mac);
	if (ret != 0) {
		pr_error("Caution: mac from chipid fail!\n");
		pr_error("Use the factory fix mac.\n");
		sprintf(mac, "%02x:%02x:%02x:%02x:%02x:%02x", 0x4a, 0xe7, 0x13, 0x36, 0xe4, 0x4b);
		mac[1] = '8'; /* (2 << 2), genarate by chipid */
	} else {
		mac[1] = '8'; /* (2 << 2), genarate by chipid */
		goto out;
	}

out:
	mac[0] = 'f';     /* platform not set */
	mac[3] = 'f';     /* net port not set */

	ret = setenv("androidboot.mac", mac);
	if (ret != 0) {
		pr_error("Caution: androidboot.mac set fail!\n");
		return -1;
	}

	pr_msg("%s, final, mac = %s\n", __func__, mac);

	return 0;
}

