#include "hw_display.h"
#include <lvgl.h>
#include <TFT_eSPI.h>

// Display configuration for ESP32-2432S028
// Physical display: 240x320 portrait, rotated to 320x240 landscape
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define LVGL_BUFFER_SIZE (SCREEN_WIDTH * 40)
#define TFT_BL 21  // Backlight pin

// TFT and LVGL objects
static TFT_eSPI tft = TFT_eSPI();
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[LVGL_BUFFER_SIZE];
static lv_disp_drv_t disp_drv;

// Display flushing callback for LVGL
static void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t *)&color_p->full, w * h, true);
    tft.endWrite();

    lv_disp_flush_ready(disp);
}

bool hw_display_init() {
    Serial.println("[HW_DISPLAY] Initializing display...");

    // Initialize TFT display
    tft.begin();
    tft.setRotation(1); // Landscape mode
    tft.fillScreen(TFT_BLACK);
    Serial.println("[HW_DISPLAY] TFT initialized (320x240 landscape)");

    // Turn on backlight
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);
    Serial.println("[HW_DISPLAY] Backlight enabled");

    // Initialize LVGL
    lv_init();
    Serial.println("[HW_DISPLAY] LVGL initialized");

    // Setup display buffer
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, LVGL_BUFFER_SIZE);

    // Initialize and register display driver
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = SCREEN_WIDTH;
    disp_drv.ver_res = SCREEN_HEIGHT;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    Serial.println("[HW_DISPLAY] Display driver registered");
    Serial.println("[HW_DISPLAY] Initialization complete");
    
    return true;
}

void hw_display_tick() {
    static unsigned long last_tick_log = 0;
    unsigned long now = millis();
    
    // Debug: Log every 5 seconds to verify this is being called
    if (now - last_tick_log > 5000) {
        Serial.println("[HW_DISPLAY] Tick running...");
        last_tick_log = now;
    }
    
    // Tell LVGL that 5ms have elapsed (critical for input device polling!)
    lv_tick_inc(5);
    
    // Handle LVGL tasks including input device polling
    lv_timer_handler();
}
