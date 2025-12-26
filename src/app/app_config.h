#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include <Arduino.h>
#include "../hw/hw_power.h"

// Application configuration - defaults and settings (Task 3.2)

// Maximum number of symbols supported
#define MAX_SYMBOLS 10

// Exchange selection
enum Exchange {
    EXCHANGE_BINANCE = 0,
    EXCHANGE_COINBASE,
    EXCHANGE_KRAKEN,
    EXCHANGE_AUTO  // Auto-select best available exchange
};

// Symbol configuration
struct SymbolConfig {
    char display_name[16];         // e.g., "BTC/USDT"
    char binance_symbol[16];       // e.g., "BTCUSDT"
    char coinbase_product[16];     // e.g., "BTC-USD"
    char kraken_pair[16];          // e.g., "XXBTZUSD"
    Exchange primary_exchange;     // Primary exchange to use
    Exchange secondary_exchange;   // Fallback if primary fails
    bool enabled;                  // Whether this symbol is active
    
    SymbolConfig() : primary_exchange(EXCHANGE_BINANCE),
                    secondary_exchange(EXCHANGE_COINBASE),
                    enabled(true) {
        display_name[0] = '\0';
        binance_symbol[0] = '\0';
        coinbase_product[0] = '\0';
        kraken_pair[0] = '\0';
    }
};

// Application configuration structure
struct AppConfig {
    // Symbol list (up to 10 symbols)
    SymbolConfig symbols[MAX_SYMBOLS];
    int num_symbols;  // Number of configured symbols (enabled or disabled)
    
    // Refresh intervals (milliseconds)
    uint32_t price_refresh_ms;
    uint32_t funding_refresh_ms;
    
    // Alert thresholds
    double spread_alert_pct;     // Alert when spread exceeds this percentage
    double funding_alert_pct;    // Alert when funding rate exceeds this percentage
    
    // Stale data detection
    uint32_t stale_ms;           // Mark data stale after this duration
    
    // Power management
    PowerMode power_mode;        // Power mode (Normal/Battery Saver/Deep Sleep)
    
    AppConfig() : num_symbols(3),
                  price_refresh_ms(5000),
                  funding_refresh_ms(60000),
                  spread_alert_pct(0.5),
                  funding_alert_pct(0.01),
                  stale_ms(15000),
                  power_mode(POWER_NORMAL) {}
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
PowerMode config_get_power_mode();
void config_set_power_mode(PowerMode mode);

// Symbol management helpers
int config_get_enabled_symbol_count();  // Count of enabled symbols
int config_get_next_enabled_symbol(int current);  // Next enabled symbol index
int config_get_prev_enabled_symbol(int current);  // Previous enabled symbol index
void config_set_symbol_enabled(int idx, bool enabled);
bool config_is_symbol_enabled(int idx);
void config_set_symbol_exchange(int idx, Exchange primary, Exchange secondary);

#endif // APP_CONFIG_H
