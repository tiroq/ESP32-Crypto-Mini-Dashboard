#include <Arduino.h>
#include <lvgl.h>
#include "hw/hw_display.h"
#include "hw/hw_touch.h"
#include "ui/ui_root.h"
#include "app/app_config.h"
#include "app/app_model.h"
#include "net/net_wifi.h"

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
    
    // Handle LVGL display tasks
    hw_display_tick();
    delay(5);
}
