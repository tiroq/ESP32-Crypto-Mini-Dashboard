#include "app_config.h"
#include "../config.h"
#include "../hw/hw_storage.h"

// Global configuration instance
static AppConfig g_config;

void config_init() {
    // Initialize all 10 symbols (3 enabled by default, 7 disabled)
    
    // Enabled symbols
    // BTC
    g_config.symbols[0].display_name = "BTC/USDT";
    g_config.symbols[0].binance_symbol = "BTCUSDT";
    g_config.symbols[0].coinbase_product = "BTC-USD";
    g_config.symbols[0].enabled = true;
    
    // ETH
    g_config.symbols[1].display_name = "ETH/USDT";
    g_config.symbols[1].binance_symbol = "ETHUSDT";
    g_config.symbols[1].coinbase_product = "ETH-USD";
    g_config.symbols[1].enabled = true;
    
    // SOL
    g_config.symbols[2].display_name = "SOL/USDT";
    g_config.symbols[2].binance_symbol = "SOLUSDT";
    g_config.symbols[2].coinbase_product = "SOL-USD";
    g_config.symbols[2].enabled = true;
    
    // Disabled symbols (available but not tracked by default)
    // XRP
    g_config.symbols[3].display_name = "XRP/USDT";
    g_config.symbols[3].binance_symbol = "XRPUSDT";
    g_config.symbols[3].coinbase_product = "XRP-USD";
    g_config.symbols[3].enabled = false;
    
    // ADA
    g_config.symbols[4].display_name = "ADA/USDT";
    g_config.symbols[4].binance_symbol = "ADAUSDT";
    g_config.symbols[4].coinbase_product = "ADA-USD";
    g_config.symbols[4].enabled = false;
    
    // DOGE
    g_config.symbols[5].display_name = "DOGE/USDT";
    g_config.symbols[5].binance_symbol = "DOGEUSDT";
    g_config.symbols[5].coinbase_product = "DOGE-USD";
    g_config.symbols[5].enabled = false;
    
    // MATIC
    g_config.symbols[6].display_name = "MATIC/USDT";
    g_config.symbols[6].binance_symbol = "MATICUSDT";
    g_config.symbols[6].coinbase_product = "MATIC-USD";
    g_config.symbols[6].enabled = false;
    
    // DOT
    g_config.symbols[7].display_name = "DOT/USDT";
    g_config.symbols[7].binance_symbol = "DOTUSDT";
    g_config.symbols[7].coinbase_product = "DOT-USD";
    g_config.symbols[7].enabled = false;
    
    // LINK
    g_config.symbols[8].display_name = "LINK/USDT";
    g_config.symbols[8].binance_symbol = "LINKUSDT";
    g_config.symbols[8].coinbase_product = "LINK-USD";
    g_config.symbols[8].enabled = false;
    
    // AVAX
    g_config.symbols[9].display_name = "AVAX/USDT";
    g_config.symbols[9].binance_symbol = "AVAXUSDT";
    g_config.symbols[9].coinbase_product = "AVAX-USD";
    g_config.symbols[9].enabled = false;
    
    g_config.num_symbols = MAX_SYMBOLS;
    
    // Refresh intervals
    g_config.price_refresh_ms = 5000;      // 5 seconds
    g_config.funding_refresh_ms = 60000;   // 60 seconds
    
    // Alert thresholds
    g_config.spread_alert_pct = 0.5;       // 0.5% spread
    g_config.funding_alert_pct = 0.01;     // 0.01% funding rate (0.01% = 0.0001)
    
    // Stale data detection (3x price refresh interval to allow for retries/delays)
    g_config.stale_ms = 30000;             // 30 seconds (3x price refresh)
    
    // Power management
    g_config.power_mode = POWER_NORMAL;    // Normal mode by default
    
    // Count enabled symbols
    int enabled_count = 0;
    for (int i = 0; i < MAX_SYMBOLS; i++) {
        if (g_config.symbols[i].enabled) enabled_count++;
    }
    
    DEBUG_PRINTLN("[CONFIG] Initialized with defaults:");
    DEBUG_PRINTF("[CONFIG]   Symbols: %d total, %d enabled\n", g_config.num_symbols, enabled_count);
    DEBUG_PRINTF("[CONFIG]   Price refresh: %lu ms\n", g_config.price_refresh_ms);
    DEBUG_PRINTF("[CONFIG]   Funding refresh: %lu ms\n", g_config.funding_refresh_ms);
    DEBUG_PRINTF("[CONFIG]   Spread alert: %.2f%%\n", g_config.spread_alert_pct);
    DEBUG_PRINTF("[CONFIG]   Funding alert: %.4f%%\n", g_config.funding_alert_pct);
    DEBUG_PRINTF("[CONFIG]   Stale threshold: %lu ms\n", g_config.stale_ms);
}

bool config_load() {
    // Try to load from storage
    if (hw_storage_load_config(&g_config)) {
        DEBUG_PRINTLN("[CONFIG] Configuration loaded from NVS");
        
        // Force update stale threshold if it's still at old default
        if (g_config.stale_ms == 15000) {
            g_config.stale_ms = 30000;
            DEBUG_PRINTLN("[CONFIG] Updated stale threshold: 15s -> 30s");
            config_save();  // Save updated config
        }
        
        return true;
    } else {
        DEBUG_PRINTLN("[CONFIG] No saved config or version mismatch - using defaults");
        return false;
    }
}

bool config_save() {
    if (hw_storage_save_config(&g_config)) {
        DEBUG_PRINTLN("[CONFIG] Configuration saved successfully");
        return true;
    } else {
        DEBUG_PRINTLN("[CONFIG] ERROR: Failed to save configuration");
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
    DEBUG_PRINTF("[CONFIG] Price refresh updated to %lu ms\n", ms);
}

void config_set_funding_refresh_ms(uint32_t ms) {
    g_config.funding_refresh_ms = ms;
    DEBUG_PRINTF("[CONFIG] Funding refresh updated to %lu ms\n", ms);
}

void config_set_spread_alert_pct(double pct) {
    g_config.spread_alert_pct = pct;
    DEBUG_PRINTF("[CONFIG] Spread alert updated to %.2f%%\n", pct);
}

void config_set_funding_alert_pct(double pct) {
    g_config.funding_alert_pct = pct;
    DEBUG_PRINTF("[CONFIG] Funding alert updated to %.4f%%\n", pct);
}

PowerMode config_get_power_mode() {
    return g_config.power_mode;
}

void config_set_power_mode(PowerMode mode) {
    g_config.power_mode = mode;
    DEBUG_PRINTF("[CONFIG] Power mode updated to %d\n", mode);
}

// Symbol management helpers

int config_get_enabled_symbol_count() {
    int count = 0;
    for (int i = 0; i < g_config.num_symbols; i++) {
        if (g_config.symbols[i].enabled) {
            count++;
        }
    }
    return count;
}

bool config_is_symbol_enabled(int idx) {
    if (idx >= 0 && idx < g_config.num_symbols) {
        return g_config.symbols[idx].enabled;
    }
    return false;
}

int config_get_next_enabled_symbol(int current_idx) {
    // Start from next index
    int idx = (current_idx + 1) % g_config.num_symbols;
    int iterations = 0;
    
    // Find next enabled symbol (with wraparound)
    while (!g_config.symbols[idx].enabled && iterations < g_config.num_symbols) {
        idx = (idx + 1) % g_config.num_symbols;
        iterations++;
    }
    
    // If we found an enabled symbol, return it; otherwise return current
    return g_config.symbols[idx].enabled ? idx : current_idx;
}

int config_get_prev_enabled_symbol(int current_idx) {
    // Start from previous index (with wraparound)
    int idx = (current_idx - 1 + g_config.num_symbols) % g_config.num_symbols;
    int iterations = 0;
    
    // Find previous enabled symbol (with wraparound)
    while (!g_config.symbols[idx].enabled && iterations < g_config.num_symbols) {
        idx = (idx - 1 + g_config.num_symbols) % g_config.num_symbols;
        iterations++;
    }
    
    // If we found an enabled symbol, return it; otherwise return current
    return g_config.symbols[idx].enabled ? idx : current_idx;
}
