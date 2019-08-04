#include <app.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <platform/iomap.h>
#include <platform.h>

#include <decompress.h>

#include <kernel/mutex.h>

#include <lib/fs.h>

#include <dev/fbcon.h>

#include "aboot/aboot.h"
#include "aboot/bootimg.h"

#include "menu.h"
#include "fs_util.h"
#include "config.h"

int boot_linux_from_ext2(void) {
	void *kernel_addr = VA((addr_t)(ABOOT_FORCE_KERNEL64_ADDR));
	void *ramdisk_addr = VA((addr_t)(ABOOT_FORCE_RAMDISK_ADDR));
	void *tags_addr = VA((addr_t)(ABOOT_FORCE_TAGS_ADDR));
	unsigned char cmdline[BOOT_ARGS_SIZE] = "earlycon=msm_serial_dm,0x78b0000 console=ttyMSM0,115200,n8 PMOS_NO_OUTPUT_REDIRECT ignore_loglevel drm.debug=12 debug dyndbg=\"file *firmware_loader/main.c +p;\"";

	unsigned char *kernel_raw = NULL;
	off_t kernel_raw_size = 0;
	off_t ramdisk_size = 0;
	off_t dtb_size = 0;

	unsigned int dev_null;

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
	
	return -1; //something went wrong
}

void reboot_init(const struct app_descriptor *app)
{
	int ret;
	
	bool boot_into_fastboot = aboot_init();

	if (!boot_into_fastboot) {
		//boot_linux_from_ext2();
		
		dprintf(CRITICAL, "ERROR: Booting default entry failed. Forcibly bringing up menu.\n");
	}

	start_fastboot();

	struct boot_entry *entry_list = NULL;
	ret = parse_boot_entries(&entry_list);
	if (ret < 0) {
		printf("falied to parse boot entries: %d\n", ret);
		return;
	}

	thread_t *thread = thread_create("menu_thread", &menu_thread, entry_list, DEFAULT_PRIORITY, DEFAULT_STACK_SIZE);
	if (thread)
		printf("thread_resume ret: %d\n", thread_resume(thread));
	else
		printf("`thread_create` failed\n");
}

APP_START(reboot)
	.init = reboot_init,
APP_END
