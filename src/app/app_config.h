#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include <Arduino.h>

// Application configuration - defaults and settings (Task 3.2)

// Symbol configuration
struct SymbolConfig {
    const char* display_name;      // e.g., "BTC/USDT"
    const char* binance_symbol;    // e.g., "BTCUSDT"
    const char* coinbase_product;  // e.g., "BTC-USD"
};

// Application configuration structure
struct AppConfig {
    // Symbol list
    SymbolConfig symbols[3];
    int num_symbols;
    
    // Refresh intervals (milliseconds)
    uint32_t price_refresh_ms;
    uint32_t funding_refresh_ms;
    
    // Alert thresholds
    double spread_alert_pct;     // Alert when spread exceeds this percentage
    double funding_alert_pct;    // Alert when funding rate exceeds this percentage
    
    // Stale data detection
    uint32_t stale_ms;           // Mark data stale after this duration
    
    AppConfig() : num_symbols(3),
                  price_refresh_ms(5000),
                  funding_refresh_ms(60000),
                  spread_alert_pct(0.5),
                  funding_alert_pct(0.01),
                  stale_ms(15000) {}
};

// Initialize configuration with defaults
void config_init();

// Load configuration from storage (NVS) - Task 10.1
// Returns true if loaded from storage, false if using defaults
bool config_load();

// Save current configuration to storage (NVS) - Task 10.1
// Returns true on success, false on error
bool config_save();

// Get the current configuration (returns reference, not thread-safe - read-only)
const AppConfig& config_get();

// Get specific configuration values (convenience getters)
int config_get_num_symbols();
const SymbolConfig* config_get_symbol(int idx);
uint32_t config_get_price_refresh_ms();
uint32_t config_get_funding_refresh_ms();
double config_get_spread_alert_pct();
double config_get_funding_alert_pct();
uint32_t config_get_stale_ms();

// Set configuration values (setters for future use)
void config_set_price_refresh_ms(uint32_t ms);
void config_set_funding_refresh_ms(uint32_t ms);
void config_set_spread_alert_pct(double pct);
void config_set_funding_alert_pct(double pct);

#endif // APP_CONFIG_H
