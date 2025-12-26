#ifndef HW_DISPLAY_H
#define HW_DISPLAY_H

#include <Arduino.h>
#include <lvgl.h>

// Display hardware abstraction
// Manages LVGL display driver initialization and TFT_eSPI integration

bool hw_display_init();
void hw_display_tick();
uint16_t hw_display_read_pixel(int32_t x, int32_t y);
void hw_display_read_rect(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t* data);

// Screenshot capture support
void hw_display_start_capture(void (*callback)(int32_t x, int32_t y, int32_t w, int32_t h, const lv_color_t* pixels));
void hw_display_stop_capture();

#endif // HW_DISPLAY_H
