#ifndef __REBOOT_GFX_H
#define __REBOOT_GFX_H

void gfx_clear(void);
void gfx_draw_pixel(unsigned int x, unsigned int y, uint32_t color);
void gfx_draw_horizontal_line(unsigned int x1, unsigned int x2, unsigned int y, uint32_t color);
void gfx_draw_vertical_line(unsigned int x, unsigned int y1, unsigned int y2, uint32_t color);
void gfx_draw_rectangle(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, uint32_t color);
void gfx_draw_filled_rectangle(unsigned int x, unsigned int y, unsigned int width, unsigned int height, uint32_t color);
void gfx_draw_text(unsigned int x, unsigned int y, const char *text, unsigned int scale_factor, uint32_t color);

#endif
