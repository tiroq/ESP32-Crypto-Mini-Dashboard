#include "app_alerts.h"
#include "../config.h"
#include "app_config.h"
#include "app_model.h"
#include "../hw/hw_alert.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// Alert cooldown configuration (Task 9.1)
static const uint32_t ALERT_COOLDOWN_MS = 30000;  // 30 seconds between alerts per symbol
static const uint32_t ALERT_CHECK_INTERVAL_MS = 300;  // Check every 300ms

// Per-symbol cooldown state
struct AlertCooldown {
    unsigned long last_alert_spread_ms;
    unsigned long last_alert_funding_ms;
    bool spread_alert_active;
    bool funding_alert_active;
    
    AlertCooldown() : last_alert_spread_ms(0), last_alert_funding_ms(0),
                      spread_alert_active(false), funding_alert_active(false) {}
};

static AlertCooldown g_alert_cooldowns[3];  // One per symbol
static int g_active_alert_count = 0;

void alerts_init() {
    // Reset all cooldowns
    for (int i = 0; i < 3; i++) {
        g_alert_cooldowns[i] = AlertCooldown();
    }
    g_active_alert_count = 0;
    
    DEBUG_PRINTLN("[ALERTS] Alert engine initialized");
    DEBUG_PRINTF("[ALERTS] Cooldown: %lu ms, Check interval: %lu ms\n", 
                  ALERT_COOLDOWN_MS, ALERT_CHECK_INTERVAL_MS);
}

bool alerts_is_active() {
    return g_active_alert_count > 0;
}

int alerts_get_active_count() {
    return g_active_alert_count;
}

/**
 * @brief Check spread threshold for a symbol
 * @param symbol_idx Symbol index
 * @param state Symbol state
 * @param now Current time in milliseconds
 * @return true if alert triggered
 */
static bool check_spread_alert(int symbol_idx, const SymbolState& state, unsigned long now) {
    AlertCooldown& cooldown = g_alert_cooldowns[symbol_idx];
    
    // Check if cooldown period has passed
    if ((now - cooldown.last_alert_spread_ms) < ALERT_COOLDOWN_MS) {
        return cooldown.spread_alert_active;  // Still in cooldown
    }
    
    // Check threshold
    if (state.spread_valid && state.spread_pct > config_get_spread_alert_pct()) {
        // Threshold exceeded - trigger alert
        DEBUG_PRINTF("[ALERTS] %s spread alert: %.2f%% exceeds threshold %.2f%%\n",
                     state.symbol_name, state.spread_pct, config_get_spread_alert_pct());
        
        // Trigger beep (200ms on, 100ms off, 2 times)
        hw_alert_beep(200, 100, 2);
        
        // Update cooldown
        cooldown.last_alert_spread_ms = now;
        cooldown.spread_alert_active = true;
        
        return true;
    } else {
        // Threshold not exceeded - clear active flag
        cooldown.spread_alert_active = false;
        return false;
    }
}

/**
 * @brief Check funding rate threshold for a symbol
 * @param symbol_idx Symbol index
 * @param state Symbol state
 * @param now Current time in milliseconds
 * @return true if alert triggered
 */
static bool check_funding_alert(int symbol_idx, const SymbolState& state, unsigned long now) {
    AlertCooldown& cooldown = g_alert_cooldowns[symbol_idx];
    
    // Check if cooldown period has passed
    if ((now - cooldown.last_alert_funding_ms) < ALERT_COOLDOWN_MS) {
        return cooldown.funding_alert_active;  // Still in cooldown
    }
    
    // Check threshold (absolute value for both positive and negative rates)
    if (state.funding.valid && fabs(state.funding.rate) > config_get_funding_alert_pct()) {
        // Threshold exceeded - trigger alert
        DEBUG_PRINTF("[ALERTS] %s funding alert: %.4f%% exceeds threshold %.4f%%\n",
                     state.symbol_name, state.funding.rate * 100.0, 
                     config_get_funding_alert_pct() * 100.0);
        
        // Trigger beep (300ms on, 150ms off, 3 times)
        hw_alert_beep(300, 150, 3);
        
        // Update cooldown
        cooldown.last_alert_funding_ms = now;
        cooldown.funding_alert_active = true;
        
        return true;
    } else {
        // Threshold not exceeded - clear active flag
        cooldown.funding_alert_active = false;
        return false;
    }
}

void alert_task(void* pvParameters) {
    DEBUG_PRINTLN("[ALERTS] Alert task started");
    
    while (true) {
        // Get current state snapshot
        AppState snapshot = model_snapshot();
        unsigned long now = millis();
        
        // Suppress alerts if data is stale (Task 8.2)
        if (snapshot.data_stale) {
            // Clear all active alert flags during stale data
            g_active_alert_count = 0;
            vTaskDelay(pdMS_TO_TICKS(ALERT_CHECK_INTERVAL_MS));
            continue;
        }
        
        // Check each symbol for threshold violations
        int active_count = 0;
        for (int i = 0; i < config_get_num_symbols(); i++) {
            const SymbolState& state = snapshot.symbols[i];
            
            bool spread_active = check_spread_alert(i, state, now);
            bool funding_active = check_funding_alert(i, state, now);
            
            if (spread_active || funding_active) {
                active_count++;
            }
        }
        
        g_active_alert_count = active_count;
        
        // Yield to other tasks
        vTaskDelay(pdMS_TO_TICKS(ALERT_CHECK_INTERVAL_MS));
    }
}
