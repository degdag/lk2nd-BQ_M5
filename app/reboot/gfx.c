// SPDX-License-Identifier: GPL-2.0+
// © 2019 Mis012

#include <dev/fbcon.h>

extern struct fbcon_config *config;
extern unsigned int *font5x12;

void gfx_clear(void)
{
	//we know that BGCOLOR is black, so we can minimize flicker by using memset
	unsigned int count = config->width * config->height * (config->bpp/8);
	memset(config->base, 0, count);
}

void gfx_draw_pixel(unsigned int x, unsigned int y, uint32_t color)
{
	if((x > (config->width - 1)) || (y > (config->height)))
		return;

	long location = x * (config->bpp/8) + y * config->width * (config->bpp/8);

	/*BGR*/
	*((uint8_t *)config->base + location + 0) = ((color >> 0) & 0xff);
	*((uint8_t *)config->base + location + 1) = ((color >> 8)  & 0xff);
	*((uint8_t *)config->base + location + 2) = ((color >> 16)  & 0xff);
}

void gfx_draw_horizontal_line(unsigned int x1, unsigned int x2, unsigned int y, uint32_t color)
{
	if(x1 > x2)
		return;

	unsigned int i;
	for (i = x1; i < x2; i++)
		gfx_draw_pixel(i, y, color);
}

void gfx_draw_vertical_line(unsigned int x, unsigned int y1, unsigned int y2, uint32_t color)
{
	if(y1 > y2)
		return;

	unsigned int i;
	for (i = y1; i < y2;i++)
		gfx_draw_pixel(x, i, color);
}

void gfx_draw_rectangle(unsigned int x1, unsigned int y1, unsigned int width, unsigned int height, uint32_t color)
{
	unsigned int x2 = x1 + width;
	unsigned int y2 = y1 + height;

	gfx_draw_horizontal_line(x1, x2, y1, color);
	gfx_draw_horizontal_line(x1, x2, y2, color);
	gfx_draw_vertical_line(x1, y1, y2, color);
	gfx_draw_vertical_line(x2, y1, y2, color);
}

void gfx_draw_filled_rectangle(unsigned int x, unsigned int y, unsigned int width, unsigned int height, uint32_t color) {
	unsigned int i;
	unsigned int j;

	for(j = y; j < y + height; j++) {
		for(i = x; i < x + width; i++) {
			gfx_draw_pixel(i, j, color);
		}
	}
}

void gfx_draw_text(unsigned int x, unsigned int y, const char *text, unsigned int scale_factor, uint32_t color)
{
	unsigned int i = 0;

	while (text[i] != '\0') {
		char *pixels = config->base;
		pixels += y * ((config->bpp / 8) * config->width);
		pixels += (x + (i * (FONT_WIDTH + 1) * scale_factor)) * ((config->bpp / 8));

		fbcon_drawchar(pixels, color, text[i], scale_factor);
		i++;
	}
}
