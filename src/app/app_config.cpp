#include "app_config.h"
#include "../config.h"
#include "../hw/hw_storage.h"

// Global configuration instance
static AppConfig g_config;

void config_init() {
    // Initialize symbol configurations with defaults
    // Symbol 0: BTC
    strncpy(g_config.symbols[0].display_name, "BTC/USDT", 15);
    strncpy(g_config.symbols[0].binance_symbol, "BTCUSDT", 15);
    strncpy(g_config.symbols[0].coinbase_product, "BTC-USD", 15);
    strncpy(g_config.symbols[0].kraken_pair, "XXBTZUSD", 15);
    g_config.symbols[0].primary_exchange = EXCHANGE_BINANCE;
    g_config.symbols[0].secondary_exchange = EXCHANGE_COINBASE;
    g_config.symbols[0].enabled = true;
    
    // Symbol 1: ETH
    strncpy(g_config.symbols[1].display_name, "ETH/USDT", 15);
    strncpy(g_config.symbols[1].binance_symbol, "ETHUSDT", 15);
    strncpy(g_config.symbols[1].coinbase_product, "ETH-USD", 15);
    strncpy(g_config.symbols[1].kraken_pair, "XETHZUSD", 15);
    g_config.symbols[1].primary_exchange = EXCHANGE_BINANCE;
    g_config.symbols[1].secondary_exchange = EXCHANGE_COINBASE;
    g_config.symbols[1].enabled = true;
    
    // Symbol 2: SOL
    strncpy(g_config.symbols[2].display_name, "SOL/USDT", 15);
    strncpy(g_config.symbols[2].binance_symbol, "SOLUSDT", 15);
    strncpy(g_config.symbols[2].coinbase_product, "SOL-USD", 15);
    strncpy(g_config.symbols[2].kraken_pair, "SOLUSD", 15);
    g_config.symbols[2].primary_exchange = EXCHANGE_BINANCE;
    g_config.symbols[2].secondary_exchange = EXCHANGE_COINBASE;
    g_config.symbols[2].enabled = true;
    
    // Symbol 3: XRP (disabled by default)
    strncpy(g_config.symbols[3].display_name, "XRP/USDT", 15);
    strncpy(g_config.symbols[3].binance_symbol, "XRPUSDT", 15);
    strncpy(g_config.symbols[3].coinbase_product, "XRP-USD", 15);
    strncpy(g_config.symbols[3].kraken_pair, "XXRPZUSD", 15);
    g_config.symbols[3].primary_exchange = EXCHANGE_BINANCE;
    g_config.symbols[3].secondary_exchange = EXCHANGE_COINBASE;
    g_config.symbols[3].enabled = false;
    
    // Symbol 4: ADA (disabled by default)
    strncpy(g_config.symbols[4].display_name, "ADA/USDT", 15);
    strncpy(g_config.symbols[4].binance_symbol, "ADAUSDT", 15);
    strncpy(g_config.symbols[4].coinbase_product, "ADA-USD", 15);
    strncpy(g_config.symbols[4].kraken_pair, "ADAUSD", 15);
    g_config.symbols[4].primary_exchange = EXCHANGE_BINANCE;
    g_config.symbols[4].secondary_exchange = EXCHANGE_COINBASE;
    g_config.symbols[4].enabled = false;
    
    // Symbol 5: DOGE (disabled by default)
    strncpy(g_config.symbols[5].display_name, "DOGE/USDT", 15);
    strncpy(g_config.symbols[5].binance_symbol, "DOGEUSDT", 15);
    strncpy(g_config.symbols[5].coinbase_product, "DOGE-USD", 15);
    strncpy(g_config.symbols[5].kraken_pair, "XDGUSD", 15);
    g_config.symbols[5].primary_exchange = EXCHANGE_BINANCE;
    g_config.symbols[5].secondary_exchange = EXCHANGE_COINBASE;
    g_config.symbols[5].enabled = false;
    
    // Symbol 6: MATIC (disabled by default)
    strncpy(g_config.symbols[6].display_name, "MATIC/USDT", 15);
    strncpy(g_config.symbols[6].binance_symbol, "MATICUSDT", 15);
    strncpy(g_config.symbols[6].coinbase_product, "MATIC-USD", 15);
    strncpy(g_config.symbols[6].kraken_pair, "MATICUSD", 15);
    g_config.symbols[6].primary_exchange = EXCHANGE_BINANCE;
    g_config.symbols[6].secondary_exchange = EXCHANGE_COINBASE;
    g_config.symbols[6].enabled = false;
    
    // Symbol 7: DOT (disabled by default)
    strncpy(g_config.symbols[7].display_name, "DOT/USDT", 15);
    strncpy(g_config.symbols[7].binance_symbol, "DOTUSDT", 15);
    strncpy(g_config.symbols[7].coinbase_product, "DOT-USD", 15);
    strncpy(g_config.symbols[7].kraken_pair, "DOTUSD", 15);
    g_config.symbols[7].primary_exchange = EXCHANGE_BINANCE;
    g_config.symbols[7].secondary_exchange = EXCHANGE_COINBASE;
    g_config.symbols[7].enabled = false;
    
    // Symbol 8: LINK (disabled by default)
    strncpy(g_config.symbols[8].display_name, "LINK/USDT", 15);
    strncpy(g_config.symbols[8].binance_symbol, "LINKUSDT", 15);
    strncpy(g_config.symbols[8].coinbase_product, "LINK-USD", 15);
    strncpy(g_config.symbols[8].kraken_pair, "LINKUSD", 15);
    g_config.symbols[8].primary_exchange = EXCHANGE_BINANCE;
    g_config.symbols[8].secondary_exchange = EXCHANGE_COINBASE;
    g_config.symbols[8].enabled = false;
    
    // Symbol 9: AVAX (disabled by default)
    strncpy(g_config.symbols[9].display_name, "AVAX/USDT", 15);
    strncpy(g_config.symbols[9].binance_symbol, "AVAXUSDT", 15);
    strncpy(g_config.symbols[9].coinbase_product, "AVAX-USD", 15);
    strncpy(g_config.symbols[9].kraken_pair, "AVAXUSD", 15);
    g_config.symbols[9].primary_exchange = EXCHANGE_BINANCE;
    g_config.symbols[9].secondary_exchange = EXCHANGE_COINBASE;
    g_config.symbols[9].enabled = false;
    
    g_config.num_symbols = MAX_SYMBOLS;  // All 10 symbols configured (3 enabled by default)
    
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
    
    DEBUG_PRINTLN("[CONFIG] Initialized with defaults:");
    DEBUG_PRINTF("[CONFIG]   Symbols: %d total, %d enabled\n", g_config.num_symbols, config_get_enabled_symbol_count());
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

int config_get_next_enabled_symbol(int current) {
    if (g_config.num_symbols == 0) return 0;
    
    int start = current;
    int next = (current + 1) % g_config.num_symbols;
    
    // Find next enabled symbol
    while (next != start) {
        if (g_config.symbols[next].enabled) {
            return next;
        }
        next = (next + 1) % g_config.num_symbols;
    }
    
    // If we looped back, return current if enabled, otherwise 0
    return g_config.symbols[current].enabled ? current : 0;
}

int config_get_prev_enabled_symbol(int current) {
    if (g_config.num_symbols == 0) return 0;
    
    int start = current;
    int prev = (current - 1 + g_config.num_symbols) % g_config.num_symbols;
    
    // Find previous enabled symbol
    while (prev != start) {
        if (g_config.symbols[prev].enabled) {
            return prev;
        }
        prev = (prev - 1 + g_config.num_symbols) % g_config.num_symbols;
    }
    
    // If we looped back, return current if enabled, otherwise 0
    return g_config.symbols[current].enabled ? current : 0;
}

void config_set_symbol_enabled(int idx, bool enabled) {
    if (idx >= 0 && idx < g_config.num_symbols) {
        g_config.symbols[idx].enabled = enabled;
        DEBUG_PRINTF("[CONFIG] Symbol %d (%s) %s\n", idx, 
                    g_config.symbols[idx].display_name,
                    enabled ? "enabled" : "disabled");
    }
}

bool config_is_symbol_enabled(int idx) {
    if (idx >= 0 && idx < g_config.num_symbols) {
        return g_config.symbols[idx].enabled;
    }
    return false;
}

void config_set_symbol_exchange(int idx, Exchange primary, Exchange secondary) {
    if (idx >= 0 && idx < g_config.num_symbols) {
        g_config.symbols[idx].primary_exchange = primary;
        g_config.symbols[idx].secondary_exchange = secondary;
        DEBUG_PRINTF("[CONFIG] Symbol %d exchange: primary=%d, secondary=%d\n", 
                    idx, primary, secondary);
    }
}

