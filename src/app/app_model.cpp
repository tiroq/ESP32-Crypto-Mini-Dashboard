#include "app_model.h"
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

// Global state with FreeRTOS mutex protection
static AppState g_app_state;
static SemaphoreHandle_t g_model_mutex = NULL;

// Symbol configuration
static const char* SYMBOL_NAMES[] = {"BTC/USDT", "ETH/USDT", "SOL/USDT"};
static const char* BINANCE_SYMBOLS[] = {"BTCUSDT", "ETHUSDT", "SOLUSDT"};
static const char* COINBASE_PRODUCTS[] = {"BTC-USD", "ETH-USD", "SOL-USD"};
static const int NUM_SYMBOLS = 3;

void model_init() {
    // Create mutex for thread-safe access
    if (g_model_mutex == NULL) {
        g_model_mutex = xSemaphoreCreateMutex();
        if (g_model_mutex == NULL) {
            Serial.println("[MODEL] ERROR: Failed to create mutex!");
            return;
        }
    }
    
    // Initialize symbol configurations
    for (int i = 0; i < NUM_SYMBOLS; i++) {
        g_app_state.symbols[i].symbol_name = SYMBOL_NAMES[i];
        g_app_state.symbols[i].binance_symbol = BINANCE_SYMBOLS[i];
        g_app_state.symbols[i].coinbase_product = COINBASE_PRODUCTS[i];
    }
    
    g_app_state.selected_symbol_idx = 0;
    g_app_state.data_stale = true;
    g_app_state.wifi_connected = false;
    g_app_state.wifi_rssi = 0;
    strcpy(g_app_state.current_time, "--:--");
    
    Serial.println("[MODEL] Initialized with thread-safe mutex");
    Serial.printf("[MODEL] Selected symbol: %s\n", SYMBOL_NAMES[0]);
}

AppState model_snapshot() {
    AppState snapshot;
    
    if (g_model_mutex != NULL && xSemaphoreTake(g_model_mutex, portMAX_DELAY) == pdTRUE) {
        snapshot = g_app_state;  // Copy entire state
        xSemaphoreGive(g_model_mutex);
    } else {
        Serial.println("[MODEL] WARNING: Failed to acquire mutex for snapshot");
    }
    
    return snapshot;
}

void model_update_symbol(int idx, const SymbolState& s) {
    if (idx < 0 || idx >= NUM_SYMBOLS) {
        Serial.printf("[MODEL] ERROR: Invalid symbol index %d\n", idx);
        return;
    }
    
    if (g_model_mutex != NULL && xSemaphoreTake(g_model_mutex, portMAX_DELAY) == pdTRUE) {
        // Preserve configuration strings
        const char* name = g_app_state.symbols[idx].symbol_name;
        const char* binance_sym = g_app_state.symbols[idx].binance_symbol;
        const char* coinbase_prod = g_app_state.symbols[idx].coinbase_product;
        
        // Preserve history before update
        double old_history[PRICE_HISTORY_SIZE];
        int old_count = g_app_state.symbols[idx].history_count;
        int old_head = g_app_state.symbols[idx].history_head;
        for (int i = 0; i < PRICE_HISTORY_SIZE; i++) {
            old_history[i] = g_app_state.symbols[idx].price_history[i];
        }
        
        // Update state
        g_app_state.symbols[idx] = s;
        
        // Restore configuration strings
        g_app_state.symbols[idx].symbol_name = name;
        g_app_state.symbols[idx].binance_symbol = binance_sym;
        g_app_state.symbols[idx].coinbase_product = coinbase_prod;
        
        // Restore history
        for (int i = 0; i < PRICE_HISTORY_SIZE; i++) {
            g_app_state.symbols[idx].price_history[i] = old_history[i];
        }
        g_app_state.symbols[idx].history_count = old_count;
        g_app_state.symbols[idx].history_head = old_head;
        
        // Add current price to history if valid
        if (s.binance_quote.valid && s.binance_quote.price > 0) {
            g_app_state.symbols[idx].price_history[old_head] = s.binance_quote.price;
            g_app_state.symbols[idx].history_head = (old_head + 1) % PRICE_HISTORY_SIZE;
            if (old_count < PRICE_HISTORY_SIZE) {
                g_app_state.symbols[idx].history_count = old_count + 1;
            }
            Serial.printf("[MODEL] Added price %.2f to history[%d/%d] head=%d count=%d\n", 
                          s.binance_quote.price, idx, old_head, 
                          g_app_state.symbols[idx].history_head, 
                          g_app_state.symbols[idx].history_count);
        }
        
        xSemaphoreGive(g_model_mutex);
        
        Serial.printf("[MODEL] Updated symbol[%d]: %s\n", idx, name);
    } else {
        Serial.println("[MODEL] WARNING: Failed to acquire mutex for update");
    }
}

void model_set_selected(int idx) {
    if (idx < 0 || idx >= NUM_SYMBOLS) {
        Serial.printf("[MODEL] ERROR: Invalid symbol index %d\n", idx);
        return;
    }
    
    if (g_model_mutex != NULL && xSemaphoreTake(g_model_mutex, portMAX_DELAY) == pdTRUE) {
        g_app_state.selected_symbol_idx = idx;
        xSemaphoreGive(g_model_mutex);
        
        Serial.printf("[MODEL] Selected symbol: %s\n", SYMBOL_NAMES[idx]);
    } else {
        Serial.println("[MODEL] WARNING: Failed to acquire mutex for set_selected");
    }
}

int model_get_selected() {
    int idx = 0;
    
    if (g_model_mutex != NULL && xSemaphoreTake(g_model_mutex, portMAX_DELAY) == pdTRUE) {
        idx = g_app_state.selected_symbol_idx;
        xSemaphoreGive(g_model_mutex);
    } else {
        Serial.println("[MODEL] WARNING: Failed to acquire mutex for get_selected");
    }
    
    return idx;
}

const char* model_get_symbol_name(int idx) {
    if (idx >= 0 && idx < NUM_SYMBOLS) {
        return SYMBOL_NAMES[idx];
    }
    return "UNKNOWN";
}

void model_update_wifi(bool connected, int rssi) {
    if (g_model_mutex != NULL && xSemaphoreTake(g_model_mutex, portMAX_DELAY) == pdTRUE) {
        g_app_state.wifi_connected = connected;
        g_app_state.wifi_rssi = rssi;
        xSemaphoreGive(g_model_mutex);
    }
}

void model_update_time(const char* time_str) {
    if (g_model_mutex != NULL && xSemaphoreTake(g_model_mutex, portMAX_DELAY) == pdTRUE) {
        strncpy(g_app_state.current_time, time_str, sizeof(g_app_state.current_time) - 1);
        g_app_state.current_time[sizeof(g_app_state.current_time) - 1] = '\0';
        xSemaphoreGive(g_model_mutex);
    }
}

void model_set_stale(bool stale) {
    if (g_model_mutex != NULL && xSemaphoreTake(g_model_mutex, portMAX_DELAY) == pdTRUE) {
        g_app_state.data_stale = stale;
        xSemaphoreGive(g_model_mutex);
    }
}
