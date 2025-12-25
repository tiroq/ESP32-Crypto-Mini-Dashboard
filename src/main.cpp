#include <Arduino.h>
#include <lvgl.h>
#include "hw/hw_display.h"
#include "hw/hw_touch.h"
#include "ui/ui_root.h"
#include "app/app_config.h"

void setup() {
    Serial.begin(115200);
    Serial.println("\n=== Crypto Dashboard ===");
    Serial.println("Starting initialization...");

    // Initialize configuration first
    config_init();

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
    hw_display_tick();
    delay(5);
}
