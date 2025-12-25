#include "hw_storage.h"
#include "../app/app_config.h"
#include <Preferences.h>

// Storage namespace and keys
static const char* STORAGE_NAMESPACE = "crypto_dash";
static const char* KEY_VERSION = "cfg_ver";
static const uint32_t CONFIG_VERSION = 1; // Increment for breaking changes

// Configuration keys
static const char* KEY_PRICE_REFRESH = "price_ms";
static const char* KEY_FUNDING_REFRESH = "fund_ms";
static const char* KEY_SPREAD_ALERT = "spread_pct";
static const char* KEY_FUNDING_ALERT = "fund_pct";
static const char* KEY_STALE_MS = "stale_ms";

// Preferences instance
static Preferences prefs;

void hw_storage_init() {
    Serial.println("[STORAGE] Initializing NVS storage...");
    // Nothing specific needed - Preferences handles init internally
}

bool hw_storage_load_config(AppConfig* config) {
    if (!config) {
        Serial.println("[STORAGE] ERROR: Null config pointer");
        return false;
    }
    
    // Open preferences in read-only mode
    if (!prefs.begin(STORAGE_NAMESPACE, true)) {
        Serial.println("[STORAGE] Failed to open preferences (read-only)");
        return false;
    }
    
    // Check version
    uint32_t stored_version = prefs.getUInt(KEY_VERSION, 0);
    if (stored_version != CONFIG_VERSION) {
        Serial.printf("[STORAGE] Version mismatch (stored=%lu, expected=%lu) - using defaults\n",
                      stored_version, CONFIG_VERSION);
        prefs.end();
        return false;
    }
    
    // Load values (use current values as defaults if key doesn't exist)
    config->price_refresh_ms = prefs.getUInt(KEY_PRICE_REFRESH, config->price_refresh_ms);
    config->funding_refresh_ms = prefs.getUInt(KEY_FUNDING_REFRESH, config->funding_refresh_ms);
    config->spread_alert_pct = prefs.getDouble(KEY_SPREAD_ALERT, config->spread_alert_pct);
    config->funding_alert_pct = prefs.getDouble(KEY_FUNDING_ALERT, config->funding_alert_pct);
    config->stale_ms = prefs.getUInt(KEY_STALE_MS, config->stale_ms);
    
    prefs.end();
    
    Serial.println("[STORAGE] Configuration loaded from NVS:");
    Serial.printf("[STORAGE]   Price refresh: %lu ms\n", config->price_refresh_ms);
    Serial.printf("[STORAGE]   Funding refresh: %lu ms\n", config->funding_refresh_ms);
    Serial.printf("[STORAGE]   Spread alert: %.2f%%\n", config->spread_alert_pct);
    Serial.printf("[STORAGE]   Funding alert: %.4f%%\n", config->funding_alert_pct);
    Serial.printf("[STORAGE]   Stale threshold: %lu ms\n", config->stale_ms);
    
    return true;
}

bool hw_storage_save_config(const AppConfig* config) {
    if (!config) {
        Serial.println("[STORAGE] ERROR: Null config pointer");
        return false;
    }
    
    // Open preferences in read-write mode
    if (!prefs.begin(STORAGE_NAMESPACE, false)) {
        Serial.println("[STORAGE] ERROR: Failed to open preferences (read-write)");
        return false;
    }
    
    // Save version first
    prefs.putUInt(KEY_VERSION, CONFIG_VERSION);
    
    // Save configuration values
    prefs.putUInt(KEY_PRICE_REFRESH, config->price_refresh_ms);
    prefs.putUInt(KEY_FUNDING_REFRESH, config->funding_refresh_ms);
    prefs.putDouble(KEY_SPREAD_ALERT, config->spread_alert_pct);
    prefs.putDouble(KEY_FUNDING_ALERT, config->funding_alert_pct);
    prefs.putUInt(KEY_STALE_MS, config->stale_ms);
    
    prefs.end();
    
    Serial.println("[STORAGE] Configuration saved to NVS:");
    Serial.printf("[STORAGE]   Price refresh: %lu ms\n", config->price_refresh_ms);
    Serial.printf("[STORAGE]   Funding refresh: %lu ms\n", config->funding_refresh_ms);
    Serial.printf("[STORAGE]   Spread alert: %.2f%%\n", config->spread_alert_pct);
    Serial.printf("[STORAGE]   Funding alert: %.4f%%\n", config->funding_alert_pct);
    Serial.printf("[STORAGE]   Stale threshold: %lu ms\n", config->stale_ms);
    
    return true;
}
