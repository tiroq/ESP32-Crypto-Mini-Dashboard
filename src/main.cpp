#include <Arduino.h>
#include "config.h"

#ifndef UNIT_TEST
// Main application code - excluded during unit testing

#include <lvgl.h>
#include "hw/hw_display.h"
#include "hw/hw_touch.h"
#include "ui/ui_root.h"
#if ENABLE_SCREENSHOT
#include "ui/ui_screenshot.h"
#include "tools/spiffs_download.h"
#endif
#include "app/app_config.h"
#include "app/app_model.h"
#include "app/app_scheduler.h"
#include "net/net_wifi.h"
#include "net/net_http.h"
#include "net/net_binance.h"
#include "net/net_coinbase.h"
#if ENABLE_OTA
#include "net/net_ota.h"
#endif

// Test flag to run HTTP test once
static bool http_test_done = false;

void setup() {
    DEBUG_BEGIN(115200);
    DEBUG_PRINTLN("\n=== Crypto Dashboard ===");
    DEBUG_PRINTLN("Starting initialization...");

    // Initialize configuration first
    config_init();

    // Initialize Wi-Fi (non-blocking)
    net_wifi_init();

    // Initialize display hardware and LVGL
    if (!hw_display_init()) {
        DEBUG_PRINTLN("[MAIN] Display initialization failed!");
        while (1) {
            delay(1000);
        }
    }

    // Initialize UI and load Dashboard screen
    ui_root_init();

    // Initialize touch input AFTER UI is created
    if (!hw_touch_init()) {
        DEBUG_PRINTLN("[MAIN] Touch initialization failed!");
        while (1) {
            delay(1000);
        }
    }
    
#if ENABLE_SCREENSHOT
    // Initialize SPIFFS for screenshots
    ui_screenshot_init();
#endif

#if ENABLE_OTA
    // Initialize OTA update server
    if (ota_init()) {
        DEBUG_PRINTLN("[MAIN] OTA server initialized");
    } else {
        DEBUG_PRINTLN("[MAIN] OTA server failed to start");
    }
#endif

    DEBUG_PRINTLN("[MAIN] Setup complete\n");
    
    // Start scheduler tasks (net_task for periodic fetching)
    scheduler_init();
}

void loop() {
    // Ensure Wi-Fi is connected (non-blocking, rate-limited)
    net_wifi_ensure_connected();
    
    // Update model with Wi-Fi status every loop iteration
    model_update_wifi(net_wifi_is_connected(), net_wifi_rssi());
    
#if ENABLE_OTA
    // Handle OTA server requests
    ota_handle();
#endif
    
#if ENABLE_SCREENSHOT
    // Check for serial commands (SCREENSHOT, DOWNLOAD, LIST)
    spiffs_check_download_command();
#endif
    
    // Handle LVGL display tasks - KEEP UI LOOP CLEAN, NO NETWORK CALLS
    // Network fetching happens in scheduler net_task
    hw_display_tick();
    delay(10);
}

#endif // UNIT_TEST
