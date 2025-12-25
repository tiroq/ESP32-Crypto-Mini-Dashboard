#include "net_wifi.h"
#include "../secrets.h"
#include <WiFi.h>

// Connection state tracking
static bool wifi_initialized = false;
static unsigned long last_connect_attempt = 0;
static unsigned long last_status_log = 0;

// Rate limiting constants
#define CONNECT_RETRY_INTERVAL_MS 5000   // Retry every 5 seconds
#define STATUS_LOG_INTERVAL_MS 30000     // Log status every 30 seconds
#define CONNECT_TIMEOUT_MS 10000         // Wait up to 10 seconds per attempt

void net_wifi_init() {
    if (wifi_initialized) return;
    
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);
    wifi_initialized = true;
    
    Serial.println("[WIFI] Initialized");
}

bool net_wifi_ensure_connected() {
    // Initialize if not done yet
    if (!wifi_initialized) {
        net_wifi_init();
    }
    
    // Check if already connected
    if (WiFi.status() == WL_CONNECTED) {
        // Rate-limited status logging
        unsigned long now = millis();
        if (now - last_status_log > STATUS_LOG_INTERVAL_MS) {
            Serial.printf("[WIFI] Connected to %s, IP: %s, RSSI: %d dBm\n",
                         WIFI_SSID, WiFi.localIP().toString().c_str(), WiFi.RSSI());
            last_status_log = now;
        }
        return true;
    }
    
    // Rate-limit connection attempts
    unsigned long now = millis();
    if (now - last_connect_attempt < CONNECT_RETRY_INTERVAL_MS) {
        return false; // Too soon to retry
    }
    last_connect_attempt = now;
    
    // Attempt connection
    Serial.printf("[WIFI] Connecting to %s...\n", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    
    // Wait for connection with timeout
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - start) < CONNECT_TIMEOUT_MS) {
        delay(100);
        Serial.print(".");
    }
    Serial.println();
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.printf("[WIFI] Connected! IP: %s, RSSI: %d dBm\n",
                     WiFi.localIP().toString().c_str(), WiFi.RSSI());
        last_status_log = millis();
        return true;
    } else {
        Serial.printf("[WIFI] Connection failed (status: %d)\n", WiFi.status());
        return false;
    }
}

int net_wifi_rssi() {
    if (WiFi.status() == WL_CONNECTED) {
        return WiFi.RSSI();
    }
    return -100; // Not connected
}

bool net_wifi_is_connected() {
    return WiFi.status() == WL_CONNECTED;
}
