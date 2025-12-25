#include "hw_touch.h"
#include <XPT2046_Touchscreen.h>
#include <lvgl.h>

// Touch configuration for ESP32-2432S028
#define TOUCH_CS 33
#define TOUCH_IRQ 36

// Touch calibration values (adjust if needed)
#define TOUCH_MIN_X 200
#define TOUCH_MAX_X 3700
#define TOUCH_MIN_Y 240
#define TOUCH_MAX_Y 3800

// Display dimensions (from hw_display.cpp)
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

// Touch controller
static XPT2046_Touchscreen ts(TOUCH_CS, TOUCH_IRQ);

// Debug logging with rate limiting
static unsigned long last_touch_log = 0;
#define TOUCH_LOG_INTERVAL_MS 500

// Touch reading callback for LVGL
static void touch_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
    if (ts.touched()) {
        TS_Point p = ts.getPoint();
        
        // Map touch coordinates to screen coordinates
        // Touch is read in portrait mode, map to landscape
        int16_t x = map(p.y, TOUCH_MIN_Y, TOUCH_MAX_Y, 0, SCREEN_WIDTH);
        int16_t y = map(p.x, TOUCH_MIN_X, TOUCH_MAX_X, SCREEN_HEIGHT, 0);
        
        // Clamp to screen bounds
        x = constrain(x, 0, SCREEN_WIDTH - 1);
        y = constrain(y, 0, SCREEN_HEIGHT - 1);
        
        data->point.x = x;
        data->point.y = y;
        data->state = LV_INDEV_STATE_PRESSED;
        
        // Debug logging (rate-limited)
        #if TOUCH_DEBUG
        unsigned long now = millis();
        if (now - last_touch_log > TOUCH_LOG_INTERVAL_MS) {
            Serial.printf("[HW_TOUCH] Touch at (%d, %d) raw=(%d, %d)\n", x, y, p.x, p.y);
            last_touch_log = now;
        }
        #endif
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

bool hw_touch_init() {
    Serial.println("[HW_TOUCH] Initializing touch controller...");
    
    // Initialize XPT2046 touch controller
    ts.begin();
    ts.setRotation(1); // Match display rotation (landscape)
    
    if (!ts.tirqTouched()) {
        Serial.println("[HW_TOUCH] Touch controller initialized (no touch detected)");
    } else {
        Serial.println("[HW_TOUCH] Touch controller initialized (touch detected)");
    }
    
    // Register touch input device with LVGL
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touch_read;
    lv_indev_drv_register(&indev_drv);
    
    Serial.println("[HW_TOUCH] Touch input device registered with LVGL");
    
    #if TOUCH_DEBUG
    Serial.println("[HW_TOUCH] Debug mode enabled - touch events will be logged");
    #endif
    
    Serial.println("[HW_TOUCH] Initialization complete");
    return true;
}
