// SPDX-License-Identifier: GPL-2.0+
// © 2019 Mis012

#include <app.h>

#include <stdio.h>

#include <kernel/mutex.h>

#include "aboot.h"

#include "config.h"
#include "menu.h"

void reboot_init(const struct app_descriptor *app)
{
	int ret;
	bool boot_into_fastboot = aboot_start();

	if (!boot_into_fastboot) {
		//boot_linux_from_ext2();

		dprintf(CRITICAL, "ERROR: Booting default entry failed. Forcibly bringing up menu.\n");
	}

	fastboot_start();

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
