// SPDX-License-Identifier: GPL-2.0+
// © 2019 Mis012

#include <app.h>

#include <stdio.h>
#include <string.h>

#include <dev/fbcon.h>
#include <kernel/mutex.h>

#include "aboot/aboot.h"

#include "boot.h"
#include "config.h"
#include "menu.h"

#define TIMEOUT_TEXT "press volume down for boot menu"
#define TIMEOUT_TEXT_SCALE 3

struct global_config global_config = {0};

struct boot_entry *get_entry_by_title(struct boot_entry * entry_list, char *title) {
	int num_of_boot_entries = get_entry_count();

	int i;
	for (i = 0; i < num_of_boot_entries; i++) {
		if((entry_list + i)->error)
			continue;
		if(!strcmp(title, (entry_list + i)->title))
			return entry_list + i;
	}

	return NULL;
}

extern uint32_t target_volume_down();

void handle_timeout() {
	int i;
	int num_iters = global_config.timeout * 1000 / 100; // times 1000 - sec to msec; divided by 100 - see "lower cpu stress"

	fbcon_draw_text(20, 20, TIMEOUT_TEXT, TIMEOUT_TEXT_SCALE, 0xFF0000);

	for (i = 0; i < num_iters; i++) {
		if (target_volume_down())
			return; //continue to boot menu

		thread_sleep(100); //lower cpu stress
	}

	boot_to_entry(global_config.default_entry);
	dprintf(CRITICAL, "ERROR: Booting default entry failed. Forcibly bringing up menu.\n");
}

void reboot_init(const struct app_descriptor *app)
{
	int ret;

	bool boot_into_fastboot = aboot_init();

	ret = parse_global_config(&global_config);
	if(ret < 0) {
		printf("falied to parse global config: %d\n", ret);
		start_fastboot();
		return;
	}

	struct boot_entry *entry_list = NULL;
	ret = parse_boot_entries(&entry_list);
	if (ret < 0) {
		printf("falied to parse boot entries: %d\n", ret);
		start_fastboot();
		return;
	}

	global_config.default_entry = get_entry_by_title(entry_list, global_config.default_entry_title);

	if (!boot_into_fastboot && global_config.default_entry) {
		if(global_config.timeout == 0) {
			boot_to_entry(global_config.default_entry);
			dprintf(CRITICAL, "ERROR: Booting default entry failed. Forcibly bringing up menu.\n");
		}
		else {
			handle_timeout();
		}
	}

	start_fastboot();

	thread_t *thread = thread_create("menu_thread", &menu_thread, entry_list, DEFAULT_PRIORITY, DEFAULT_STACK_SIZE);
	if (thread)
		printf("thread_resume ret: %d\n", thread_resume(thread));
	else
		printf("`thread_create` failed\n");
}

APP_START(reboot)
	.init = reboot_init,
APP_END
