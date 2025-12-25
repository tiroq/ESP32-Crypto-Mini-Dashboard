#include <Arduino.h>

#ifndef UNIT_TEST
// Main application code - excluded during unit testing

#include <lvgl.h>
#include "hw/hw_display.h"
#include "hw/hw_touch.h"
#include "ui/ui_root.h"
#include "app/app_config.h"
#include "app/app_model.h"
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
            Serial.printf("[MAIN] HTTP test SUCCESS: %d bytes\n", response.length());
            Serial.println("[MAIN] First 200 chars:");
            Serial.println(response.substring(0, 200));
        } else {
            Serial.println("[MAIN] HTTP test FAILED");
        }
        
        // Test Binance spot price (Task 6.1)
        Serial.println("\n[MAIN] Testing Binance spot price...");
        double btc_price = 0.0;
        if (net_binance::fetch_spot("BTCUSDT", &btc_price)) {
            Serial.printf("[MAIN] Binance test SUCCESS: BTC price = $%.2f\n", btc_price);
        } else {
            Serial.println("[MAIN] Binance test FAILED");
        }
        
        // Test Coinbase spot price (Task 6.2)
        Serial.println("\n[MAIN] Testing Coinbase spot price...");
        double btc_price_cb = 0.0;
        if (net_coinbase::fetch_spot("BTC-USD", &btc_price_cb)) {
            Serial.printf("[MAIN] Coinbase test SUCCESS: BTC price = $%.2f\n", btc_price_cb);
            
            // Show spread between exchanges
            double spread_abs = btc_price_cb - btc_price;
            double spread_pct = (spread_abs / btc_price) * 100.0;
            Serial.printf("[MAIN] Spread: $%.2f (%.3f%%)\n", spread_abs, spread_pct);
        } else {
            Serial.println("[MAIN] Coinbase test FAILED");
        }
        
        // Test Binance funding rate (Task 6.3)
        Serial.println("\n[MAIN] Testing Binance funding rate...");
        double btc_funding = 0.0;
        if (net_binance::fetch_funding("BTCUSDT", &btc_funding)) {
            Serial.printf("[MAIN] Funding rate test SUCCESS: %.4f%% (%.6f)\n", btc_funding * 100.0, btc_funding);
        } else {
            Serial.println("[MAIN] Funding rate test FAILED");
        }
        
        Serial.println();
    }
    
    // Handle LVGL display tasks
    hw_display_tick();
    delay(5);
}

#endif // UNIT_TEST
