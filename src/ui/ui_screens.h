#ifndef UI_SCREENS_H
#define UI_SCREENS_H

#include <Arduino.h>
#include <lvgl.h>

// UI screens - Dashboard, Alerts, Settings

// Create and return Dashboard screen
lv_obj_t* ui_screens_create_dashboard();

// Create placeholder screens
lv_obj_t* ui_screens_create_alerts();
lv_obj_t* ui_screens_create_settings();

#endif // UI_SCREENS_H
