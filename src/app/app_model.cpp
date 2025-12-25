#include "app_model.h"

// Stub model state (Task 2.2)
static int selected_symbol_idx = 0;
static const char* symbol_names[] = {"BTC/USDT", "ETH/USDT", "SOL/USDT"};
static const int NUM_SYMBOLS = 3;

void model_init() {
    selected_symbol_idx = 0;
    Serial.println("[MODEL] Initialized with BTC/USDT selected");
}

int model_get_selected() {
    return selected_symbol_idx;
}

void model_set_selected(int idx) {
    if (idx >= 0 && idx < NUM_SYMBOLS) {
        selected_symbol_idx = idx;
        Serial.printf("[MODEL] Selected symbol: %s\n", symbol_names[idx]);
    }
}

const char* model_get_symbol_name(int idx) {
    if (idx >= 0 && idx < NUM_SYMBOLS) {
        return symbol_names[idx];
    }
    return "UNKNOWN";
}
