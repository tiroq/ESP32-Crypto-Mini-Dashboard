#ifndef APP_MODEL_H
#define APP_MODEL_H

#include <Arduino.h>

// Application model - Thread-safe state management (Task 3.1)

// Data structures
struct Quote {
    double price;
    bool valid;
    unsigned long last_update_ms;
    
    Quote() : price(0.0), valid(false), last_update_ms(0) {}
};

struct Funding {
    double rate;
    bool valid;
    unsigned long last_update_ms;
    
    Funding() : rate(0.0), valid(false), last_update_ms(0) {}
};

struct SymbolState {
    const char* symbol_name;        // e.g., "BTC/USDT"
    const char* binance_symbol;     // e.g., "BTCUSDT"
    const char* coinbase_product;   // e.g., "BTC-USD"
    
    Quote binance_quote;
    Quote coinbase_quote;
    Funding funding;
    
    // Computed values
    double spread_abs;
    double spread_pct;
    bool spread_valid;
    
    SymbolState() : symbol_name(""), binance_symbol(""), coinbase_product(""),
                    spread_abs(0.0), spread_pct(0.0), spread_valid(false) {}
};

struct AppState {
    SymbolState symbols[3];  // BTC, ETH, SOL
    int selected_symbol_idx;
    bool data_stale;
    
    // Wi-Fi / network status
    bool wifi_connected;
    int wifi_rssi;
    
    // Time
    char current_time[16];  // HH:MM format
    
    AppState() : selected_symbol_idx(0), data_stale(true), 
                 wifi_connected(false), wifi_rssi(0) {
        strcpy(current_time, "--:--");
    }
};

// Thread-safe API
void model_init();

// Get a complete snapshot of the current state (thread-safe, returns copy)
AppState model_snapshot();

// Update a specific symbol's data (thread-safe)
void model_update_symbol(int idx, const SymbolState& s);

// Set currently selected symbol index (thread-safe)
void model_set_selected(int idx);

// Get currently selected symbol index (thread-safe)
int model_get_selected();

// Get symbol name by index (thread-safe)
const char* model_get_symbol_name(int idx);

// Update Wi-Fi status (thread-safe)
void model_update_wifi(bool connected, int rssi);

// Update current time (thread-safe)
void model_update_time(const char* time_str);

// Mark data as stale/fresh (thread-safe)
void model_set_stale(bool stale);

#endif // APP_MODEL_H
