#include <Arduino.h>

#ifndef UNIT_TEST
// Main application code - excluded during unit testing

#include <lvgl.h>
#include "hw/hw_display.h"
#include "hw/hw_touch.h"
#include "ui/ui_root.h"
#include "ui/ui_screenshot.h"
#include "tools/spiffs_download.h"
#include "app/app_config.h"
#include "app/app_model.h"
#include "app/app_scheduler.h"
#include "net/net_wifi.h"
#include "net/net_http.h"
#include "net/net_binance.h"
#include "net/net_coinbase.h"

// Test flag to run HTTP test once
static bool http_test_done = false;

void setup() {
    Serial.begin(115200);
    Serial.println("\n=== Crypto Dashboard ===");
    Serial.println("Starting initialization...");

    // Initialize configuration first
    config_init();

    // Initialize Wi-Fi (non-blocking)
    net_wifi_init();

    // Initialize display hardware and LVGL
    if (!hw_display_init()) {
        Serial.println("[MAIN] Display initialization failed!");
        while (1) {
            delay(1000);
        }
    }

    // Initialize UI and load Dashboard screen
    ui_root_init();

    // Initialize touch input AFTER UI is created
    if (!hw_touch_init()) {
        Serial.println("[MAIN] Touch initialization failed!");
        while (1) {
            delay(1000);
        }
    }
    
    // Initialize SPIFFS for screenshots
    ui_screenshot_init();

    Serial.println("[MAIN] Setup complete\n");
    
    // Start scheduler tasks (net_task for periodic fetching)
    scheduler_init();
}

void loop() {
    // Ensure Wi-Fi is connected (non-blocking, rate-limited)
    net_wifi_ensure_connected();
    
    // Update model with Wi-Fi status every loop iteration
    model_update_wifi(net_wifi_is_connected(), net_wifi_rssi());
    
    // Check for serial commands (SCREENSHOT, DOWNLOAD, LIST)
    spiffs_check_download_command();
    
    // Handle LVGL display tasks - KEEP UI LOOP CLEAN, NO NETWORK CALLS
    // Network fetching happens in scheduler net_task
    hw_display_tick();
    delay(10);
}

#endif // UNIT_TEST
