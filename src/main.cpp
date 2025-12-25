#include <Arduino.h>
#include <lvgl.h>
#include "hw/hw_display.h"
#include "hw/hw_touch.h"

void setup() {
    Serial.begin(115200);
    Serial.println("\n=== Crypto Dashboard ===");
    Serial.println("Starting initialization...");

    // Initialize display hardware and LVGL
    if (!hw_display_init()) {
        Serial.println("[MAIN] Display initialization failed!");
        while (1) {
            delay(1000);
        }
    }

    // Initialize touch input
    if (!hw_touch_init()) {
        Serial.println("[MAIN] Touch initialization failed!");
        while (1) {
            delay(1000);
        }
    }

    // Create a simple label for initial UI
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "Crypto Dashboard");
    lv_obj_set_style_text_font(label, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0x00FF00), 0);
    lv_obj_center(label);

    Serial.println("[MAIN] Setup complete\n");
}

void loop() {
    hw_display_tick();
    delay(5);
}
