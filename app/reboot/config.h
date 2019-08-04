#ifndef __REBOOT_BOOT_ENTRY_H
#define __REBOOT_BOOT_ENTRY_H

struct boot_entry {
	char *title;
	char *linux;
	char *initrd;
	char *options;
	bool error;
};

int get_entry_count(void);

int parse_boot_entries(struct boot_entry **entry_list);

#endif
