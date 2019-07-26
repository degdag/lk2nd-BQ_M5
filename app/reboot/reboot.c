#include <app.h>

#include <platform/iomap.h>
#include <platform.h>

#include <decompress.h>

#include <menu_keys_detect.h>
#include <display_menu.h>

#include <lib/bio.h>
#include <lib/fs.h>

#include <string.h>

#include "aboot.h"
#include "bootimg.h"

off_t fs_get_file_size(const char *filename) {
	filehandle *file_handle = NULL;
	struct file_stat file_stat;

	int ret;

	ret = fs_open_file(filename, &file_handle);
	if(ret < 0)
		return 0;

	ret = fs_stat_file(file_handle, &file_stat);
	if(ret < 0)
		return 0;

	ret = fs_close_file(file_handle);
	if (ret < 0)
		return 0;

	return file_stat.size;
}

int boot_linux_from_ext2(void) {
	void *kernel_addr = VA((addr_t)(ABOOT_FORCE_KERNEL64_ADDR));
	void *ramdisk_addr = VA((addr_t)(ABOOT_FORCE_RAMDISK_ADDR));
	void *tags_addr = VA((addr_t)(ABOOT_FORCE_TAGS_ADDR));
	unsigned char cmdline[BOOT_ARGS_SIZE] = "earlycon=msm_serial_dm,0x78b0000 console=ttyMSM0,115200,n8 PMOS_NO_OUTPUT_REDIRECT ignore_loglevel drm.debug=12 debug dyndbg=\"file *firmware_loader/main.c +p;\"";

	unsigned char *kernel_raw = NULL;
	off_t kernel_raw_size = 0;
	off_t ramdisk_size = 0;
	off_t dtb_size = 0;

	int dev_null;

	printf("booting from ext2 partition 'system'\n");

	if(fs_mount("/boot", "ext2", "hd1p24")) {
		printf("fs_mount failed\n");
		return -1;
	}

	kernel_raw_size = fs_get_file_size("/boot/Image.gz");
	if(!kernel_raw_size) {
		printf("fs_get_file_size (Image.gz) failed\n");
		return -1;
	}
	kernel_raw = ramdisk_addr; //right where the biggest possible decompressed kernel would end; sure to be out of the way

	if(fs_load_file("/boot/Image.gz", kernel_raw, kernel_raw_size) < 0) {
		printf("failed loading /boot/Image.gz at %p\n", kernel_addr);
		return -1;
	}

	if(is_gzip_package(kernel_raw, kernel_raw_size)) {
		if(decompress(kernel_raw, kernel_raw_size, kernel_addr, ABOOT_FORCE_RAMDISK_ADDR - ABOOT_FORCE_KERNEL64_ADDR, &dev_null, &dev_null)) {
			printf("kernel decompression failed\n");
			return -1;
		}
	} else {
		memmove(kernel_addr, kernel_raw, kernel_raw_size);
	}

	kernel_raw = NULL; //get rid of dangerous reference to ramdisk_addr before it can do harm

	ramdisk_size = fs_get_file_size("/boot/initramfs.cpio.gz");
	if (!ramdisk_size) {
		printf("fs_get_file_size (/boot/initramfs.cpio.gz) failed\n");
		return -1;
	}

	if(fs_load_file("/boot/initramfs.cpio.gz", ramdisk_addr, ramdisk_size) < 0) {
		printf("failed loading /boot/initramfs.cpio.gz at %p\n", ramdisk_addr);
		return -1;
	}

	dtb_size = fs_get_file_size("/boot/msm8916-samsung-a3u-eur.dtb");
	if (!ramdisk_size) {
		printf("fs_get_file_size (/boot/msm8916-samsung-a3u-eur.dtb) failed\n");
		return -1;
	}

	if(fs_load_file("/boot/msm8916-samsung-a3u-eur.dtb", tags_addr, dtb_size) < 0) {
		printf("failed loading /boot/msm8916-samsung-a3u-eur.dtb at %p\n", tags_addr);
		return -1;
	}

	fs_unmount("/boot");

	boot_linux(kernel_addr, tags_addr, (const char *)cmdline, board_machtype(), ramdisk_addr, ramdisk_size);
}

void reboot_init(const struct app_descriptor *app)
{
	bool boot_into_fastboot = aboot_start();

	if (!boot_into_fastboot) {
		boot_linux_from_ext2();

		dprintf(CRITICAL, "ERROR: Booting default entry failed. Forcibly bringing up menu.\n");
	}

	fastboot_start();
	display_fastboot_menu();
}

APP_START(reboot)
	.init = reboot_init,
APP_END
