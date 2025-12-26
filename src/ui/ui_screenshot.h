#ifndef UI_SCREENSHOT_H
#define UI_SCREENSHOT_H

#include <Arduino.h>

/**
 * Initialize SPIFFS for screenshot storage.
 * Should be called once during startup.
 * Returns true on success, false on failure.
 */
bool ui_screenshot_init();

/**
 * Take a screenshot of the currently active LVGL screen.
 * Saves the image as a BMP file to SPIFFS at the specified path.
 * 
 * @param path Full path in SPIFFS (e.g., "/screenshot.bmp")
 * @return true on success, false on failure
 */
bool ui_take_screenshot(const char* path);

#endif // UI_SCREENSHOT_H
