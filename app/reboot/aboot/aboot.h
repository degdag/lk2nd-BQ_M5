#ifndef __APP_ABOOT_H
#define __APP_ABOOT_H

void start_fastboot(void);
bool aboot_init(void);

void boot_linux(void *kernel, unsigned int *tags,
		const char *cmdline, unsigned int machtype,
		void *ramdisk, unsigned int ramdisk_size);

#endif
