#include "hw_touch.h"
#include <XPT2046_Touchscreen.h>
#include <SPI.h>
#include <lvgl.h>

// Touch configuration for ESP32-2432S028
#define TOUCH_CS 33
#define TOUCH_IRQ 36
#define TOUCH_MOSI 32
#define TOUCH_MISO 39
#define TOUCH_CLK 25

// Touch calibration values (adjust if needed)
#define TOUCH_MIN_X 200
#define TOUCH_MAX_X 3700
#define TOUCH_MIN_Y 240
#define TOUCH_MAX_Y 3800

// Display dimensions (from hw_display.cpp)
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

// Touch controller with dedicated SPI bus
SPIClass touchscreenSPI = SPIClass(VSPI);
static XPT2046_Touchscreen ts(TOUCH_CS, TOUCH_IRQ);

// Debug logging with rate limiting
static unsigned long last_touch_log = 0;
#define TOUCH_LOG_INTERVAL_MS 500

// Touch reading callback for LVGL
static void touch_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
    #if TOUCH_DEBUG
    static unsigned long last_callback_log = 0;
    unsigned long now = millis();
    
    // Log that callback is being called every 2 seconds
    if (now - last_callback_log > 2000) {
        Serial.println("[HW_TOUCH] Callback invoked by LVGL");
        last_callback_log = now;
    }
    #endif
    
    // Check both tirqTouched() and touched() like the working example
    bool touched = ts.tirqTouched() && ts.touched();
    
    #if TOUCH_DEBUG
    static bool was_touched = false;
    
    // Log touch state changes
    if (touched != was_touched) {
        Serial.printf("[HW_TOUCH] Touch state changed: %s\n", touched ? "PRESSED" : "RELEASED");
        was_touched = touched;
    }
    #endif
    
    if (touched) {
        TS_Point p = ts.getPoint();
        
        #if TOUCH_DEBUG
        Serial.printf("[HW_TOUCH] Raw touch: x=%d, y=%d, z=%d\n", p.x, p.y, p.z);
        #endif
        
        // Calibrate with map function (from working example)
        int16_t x = map(p.x, TOUCH_MIN_X, TOUCH_MAX_X, 1, SCREEN_WIDTH);
        int16_t y = map(p.y, TOUCH_MIN_Y, TOUCH_MAX_Y, 1, SCREEN_HEIGHT);
        
        // Clamp to screen bounds
        x = constrain(x, 0, SCREEN_WIDTH - 1);
        y = constrain(y, 0, SCREEN_HEIGHT - 1);
        
        data->point.x = x;
        data->point.y = y;
        data->state = LV_INDEV_STATE_PRESSED;
        
        #if TOUCH_DEBUG
        unsigned long now2 = millis();
        if (now2 - last_touch_log > TOUCH_LOG_INTERVAL_MS) {
            Serial.printf("[HW_TOUCH] Touch at screen(%d, %d) raw(%d, %d) z=%d\n", x, y, p.x, p.y, p.z);
            last_touch_log = now2;
        }
        #endif
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

bool hw_touch_init() {
    Serial.println("[HW_TOUCH] Initializing touch controller...");
    
    // Initialize dedicated SPI bus for touchscreen
    touchscreenSPI.begin(TOUCH_CLK, TOUCH_MISO, TOUCH_MOSI, TOUCH_CS);
    Serial.println("[HW_TOUCH] Touch SPI bus initialized");
    
    // Initialize XPT2046 touch controller
    ts.begin(touchscreenSPI);
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
    lv_indev_t *indev = lv_indev_drv_register(&indev_drv);
    
    if (indev) {
        Serial.println("[HW_TOUCH] Touch input device registered with LVGL");
    } else {
        Serial.println("[HW_TOUCH] ERROR: Failed to register touch input device!");
        return false;
    }
    
    #if TOUCH_DEBUG
    Serial.println("[HW_TOUCH] Debug mode enabled - touch events will be logged");
    #endif
    
    Serial.println("[HW_TOUCH] Initialization complete");
    return true;
}
