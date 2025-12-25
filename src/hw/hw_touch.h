#ifndef HW_TOUCH_H
#define HW_TOUCH_H

#include <Arduino.h>

// Touch input hardware abstraction
// Manages XPT2046 touch controller and LVGL input device integration

// Debug mode - set to true to enable touch logging
#define TOUCH_DEBUG false

bool hw_touch_init();

#endif // HW_TOUCH_H
