#ifndef __APP_ABOOT_H
#define __APP_ABOOT_H

bool aboot_start(void);
void fastboot_start(void);

void boot_linux(void *kernel, unsigned int *tags,
		const char *cmdline, unsigned int machtype,
		void *ramdisk, unsigned int ramdisk_size);

int boot_linux_from_mmc(void);

#endif
