#include "app_config.h"
#include "../hw/hw_storage.h"

// Global configuration instance
static AppConfig g_config;

void config_init() {
    // Initialize symbol configurations
    // BTC
    g_config.symbols[0].display_name = "BTC/USDT";
    g_config.symbols[0].binance_symbol = "BTCUSDT";
    g_config.symbols[0].coinbase_product = "BTC-USD";
    
    // ETH
    g_config.symbols[1].display_name = "ETH/USDT";
    g_config.symbols[1].binance_symbol = "ETHUSDT";
    g_config.symbols[1].coinbase_product = "ETH-USD";
    
    // SOL
    g_config.symbols[2].display_name = "SOL/USDT";
    g_config.symbols[2].binance_symbol = "SOLUSDT";
    g_config.symbols[2].coinbase_product = "SOL-USD";
    
    g_config.num_symbols = 3;
    
    // Refresh intervals
    g_config.price_refresh_ms = 5000;      // 5 seconds
    g_config.funding_refresh_ms = 60000;   // 60 seconds
    
    // Alert thresholds
    g_config.spread_alert_pct = 0.5;       // 0.5% spread
    g_config.funding_alert_pct = 0.01;     // 0.01% funding rate (0.01% = 0.0001)
    
    // Stale data detection (3x price refresh interval to allow for retries/delays)
    g_config.stale_ms = 30000;             // 30 seconds (3x price refresh)
    
    Serial.println("[CONFIG] Initialized with defaults:");
    Serial.printf("[CONFIG]   Symbols: %d (BTC, ETH, SOL)\n", g_config.num_symbols);
    Serial.printf("[CONFIG]   Price refresh: %lu ms\n", g_config.price_refresh_ms);
    Serial.printf("[CONFIG]   Funding refresh: %lu ms\n", g_config.funding_refresh_ms);
    Serial.printf("[CONFIG]   Spread alert: %.2f%%\n", g_config.spread_alert_pct);
    Serial.printf("[CONFIG]   Funding alert: %.4f%%\n", g_config.funding_alert_pct);
    Serial.printf("[CONFIG]   Stale threshold: %lu ms\n", g_config.stale_ms);
}

bool config_load() {
    // Try to load from storage
    if (hw_storage_load_config(&g_config)) {
        Serial.println("[CONFIG] Configuration loaded from NVS");
        
        // Force update stale threshold if it's still at old default
        if (g_config.stale_ms == 15000) {
            g_config.stale_ms = 30000;
            Serial.println("[CONFIG] Updated stale threshold: 15s -> 30s");
            config_save();  // Save updated config
        }
        
        return true;
    } else {
        Serial.println("[CONFIG] No saved config or version mismatch - using defaults");
        return false;
    }
}

bool config_save() {
    if (hw_storage_save_config(&g_config)) {
        Serial.println("[CONFIG] Configuration saved successfully");
        return true;
    } else {
        Serial.println("[CONFIG] ERROR: Failed to save configuration");
        return false;
    }
}

const AppConfig& config_get() {
    return g_config;
}

int config_get_num_symbols() {
    return g_config.num_symbols;
}

const SymbolConfig* config_get_symbol(int idx) {
    if (idx >= 0 && idx < g_config.num_symbols) {
        return &g_config.symbols[idx];
    }
    return nullptr;
}

uint32_t config_get_price_refresh_ms() {
    return g_config.price_refresh_ms;
}

uint32_t config_get_funding_refresh_ms() {
    return g_config.funding_refresh_ms;
}

double config_get_spread_alert_pct() {
    return g_config.spread_alert_pct;
}

double config_get_funding_alert_pct() {
    return g_config.funding_alert_pct;
}

uint32_t config_get_stale_ms() {
    return g_config.stale_ms;
}

void config_set_price_refresh_ms(uint32_t ms) {
    g_config.price_refresh_ms = ms;
    Serial.printf("[CONFIG] Price refresh updated to %lu ms\n", ms);
}

void config_set_funding_refresh_ms(uint32_t ms) {
    g_config.funding_refresh_ms = ms;
    Serial.printf("[CONFIG] Funding refresh updated to %lu ms\n", ms);
}

void config_set_spread_alert_pct(double pct) {
    g_config.spread_alert_pct = pct;
    Serial.printf("[CONFIG] Spread alert updated to %.2f%%\n", pct);
}

void config_set_funding_alert_pct(double pct) {
    g_config.funding_alert_pct = pct;
    Serial.printf("[CONFIG] Funding alert updated to %.4f%%\n", pct);
}
