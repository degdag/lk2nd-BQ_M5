#include <pm8x41.h>
#include <pm8x41_hw.h>
#include <kernel/thread.h>
#include <dev/fbcon.h>
#include <stdio.h>

#include "config.h"
#include "gfx.h"

int num_of_boot_entries;

struct boot_entry *entry_list;

#define BOOT_ENTRY_SCALE 2
#define ACTUAL_FONT_WIDTH (FONT_WIDTH * BOOT_ENTRY_SCALE)
#define ACTUAL_FONT_HEIGHT (FONT_HEIGHT * BOOT_ENTRY_SCALE)

int selected_option = 0;

static void draw_menu(void) {
	num_of_boot_entries = get_entry_count();

	uint32_t max_width = fbcon_get_width() - 1;
	uint32_t max_height = fbcon_get_height() - 1;

	uint32_t frame_width = max_width * 0.90;
	uint32_t frame_height = max_height * 0.90;

	uint32_t margin_x = (max_width - frame_width) / 2;
	uint32_t margin_y = (max_height - frame_height) / 2;

	uint32_t highlight_color;
	uint32_t font_color;

	gfx_draw_rectangle(margin_x, margin_y, frame_width, frame_height, 0xFFFFFF);

	int i;
	for (i = 0; i < num_of_boot_entries; i++) {
		if(i == selected_option)
			highlight_color = 0xFF0000;
		else
			highlight_color = 0x000000;
		if((entry_list + i)->error)
			font_color = 0xFF0000;
		else
			font_color = 0xFFFFFF;
		gfx_draw_filled_rectangle(margin_x + 8, (margin_y + 8) + i * (ACTUAL_FONT_HEIGHT + 4) - 2, frame_width - (2 * 8), 2 + ACTUAL_FONT_HEIGHT + 2, highlight_color);
		gfx_draw_text(margin_x + 10, (margin_y + 10) + i * (ACTUAL_FONT_HEIGHT + 4), (entry_list + i)->title, BOOT_ENTRY_SCALE, font_color);
	}

	fbcon_flush();

}

#define KEY_DETECT_FREQUENCY		50

extern int target_volume_up();
extern uint32_t target_volume_down();

static bool handle_keys(void) {
	uint32_t volume_up_pressed = target_volume_up();
	uint32_t volume_down_pressed = target_volume_down();
	uint32_t power_button_pressed = pm8x41_get_pwrkey_is_pressed();

	if(volume_up_pressed) {
		if(selected_option > 0)
			selected_option--;
		printf("volume_up_pressed\n");
		return 1;
	}

	if (volume_down_pressed) {
		if(selected_option < (num_of_boot_entries - 1))
		selected_option++;
		printf("volume_up_pressed\n");
		return 1;
	}

	if(power_button_pressed) {
		printf("[***] selected option: %d\n", selected_option);
	}

	return 0;
}

int menu_thread(void *arg) {
	entry_list = (struct boot_entry *)arg;

	fbcon_clear();
	draw_menu();

	while(1) {
		if(handle_keys()) {
			draw_menu();
			thread_sleep(100); //improve precision
		}

		thread_sleep(KEY_DETECT_FREQUENCY);
	}
}
