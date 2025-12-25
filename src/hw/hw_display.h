#ifndef HW_DISPLAY_H
#define HW_DISPLAY_H

#include <Arduino.h>

// Display hardware abstraction
// Manages LVGL display driver initialization and TFT_eSPI integration

bool hw_display_init();
void hw_display_tick();

#endif // HW_DISPLAY_H
