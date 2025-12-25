#include <Arduino.h>
#include <lvgl.h>
#include "hw/hw_display.h"
#include "hw/hw_touch.h"
#include "ui/ui_root.h"
#include "app/app_config.h"
#include "app/app_model.h"
#include "net/net_wifi.h"
#include "net/net_http.h"

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

    Serial.println("[MAIN] Setup complete\n");
}

void loop() {
    // Ensure Wi-Fi is connected (non-blocking, rate-limited)
    net_wifi_ensure_connected();
    
    // Update model with Wi-Fi status every loop iteration
    model_update_wifi(net_wifi_is_connected(), net_wifi_rssi());
    
    // Test HTTP GET once after Wi-Fi connects (Task 5.2 test)
    if (!http_test_done && net_wifi_is_connected()) {
        http_test_done = true;
        Serial.println("\n[MAIN] Testing HTTP GET...");
        
        String response;
        if (http_get("http://httpbin.org/get", response, 10000)) {
            Serial.printf("[MAIN] HTTP test SUCCESS: %d bytes\\n", response.length());
            Serial.println("[MAIN] First 200 chars:");
            Serial.println(response.substring(0, 200));
        } else {
            Serial.println("[MAIN] HTTP test FAILED");
        }
        Serial.println();
    }
    
    // Handle LVGL display tasks
    hw_display_tick();
    delay(5);
}
