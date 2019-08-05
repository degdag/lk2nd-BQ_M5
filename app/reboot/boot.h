#ifndef __REBOOT_BOOT_H
#define __REBOOT_BOOT_H

int boot_linux_from_ext2(char *kernel_path, char *ramdisk_path, char *cmdline);

#endif
