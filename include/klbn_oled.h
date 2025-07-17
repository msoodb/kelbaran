
#ifndef KLBN_OLED_H
#define KLBN_OLED_H

#include "klbn_types.h"
#include <stdint.h>

void klbn_oled_init(void);
void klbn_oled_clear(void);
void klbn_oled_flush(void);
void klbn_oled_force_full_update(void);
void klbn_oled_draw_pixel(uint8_t x, uint8_t y, uint8_t color);
void klbn_oled_draw_line(int x0, int y0, int x1, int y1);
void klbn_oled_draw_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
void klbn_oled_draw_char(uint8_t x, uint8_t page, char c);
void klbn_oled_draw_text(uint8_t x, uint8_t page, const char *str);
void klbn_oled_invert(void);
void klbn_oled_draw_progress_bar(uint8_t percent);
void klbn_oled_scroll_horizontal(const char *text, uint8_t speed);
void klbn_oled_apply(const klbn_oled_command_t *cmd);

#endif
