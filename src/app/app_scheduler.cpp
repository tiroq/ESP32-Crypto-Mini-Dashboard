#include "app_scheduler.h"
#include "app_config.h"
#include "app_model.h"
#include "app_math.h"
#include "app_alerts.h"
#include "../net/net_wifi.h"
#include "../net/net_binance.h"
#include "../net/net_coinbase.h"
#include "../hw/hw_alert.h"
#include "../hw/hw_storage.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// Task handles
static TaskHandle_t net_task_handle = NULL;
static TaskHandle_t alert_task_handle = NULL;

// Backoff configuration
static const uint32_t BACKOFF_MIN_MS = 1000;      // 1 second minimum backoff
static const uint32_t BACKOFF_MAX_MS = 60000;     // 1 minute maximum backoff
static const float BACKOFF_MULTIPLIER = 1.5;      // Exponential backoff multiplier

// Per-symbol backoff state
struct BackoffState {
    uint32_t current_delay_ms;
    unsigned long last_attempt_ms;
    
    BackoffState() : current_delay_ms(BACKOFF_MIN_MS), last_attempt_ms(0) {}
    
    void reset() {
        current_delay_ms = BACKOFF_MIN_MS;
    }
    
    void increase() {
        current_delay_ms = min((uint32_t)(current_delay_ms * BACKOFF_MULTIPLIER), BACKOFF_MAX_MS);
    }
    
    bool should_retry(unsigned long now_ms) {
        return (now_ms - last_attempt_ms) >= current_delay_ms;
    }
    
    void mark_attempt(unsigned long now_ms) {
        last_attempt_ms = now_ms;
    }
};

static BackoffState price_backoff[3];    // One per symbol
static BackoffState funding_backoff[3];  // One per symbol

// Performance tracking for stability monitoring (Task 11.1)
struct PerformanceMetrics {
    unsigned long last_price_fetch_duration_ms;
    unsigned long last_funding_fetch_duration_ms;
    unsigned long last_stability_log_ms;
    uint32_t min_free_heap;
    
    PerformanceMetrics() : last_price_fetch_duration_ms(0),
                          last_funding_fetch_duration_ms(0),
                          last_stability_log_ms(0),
                          min_free_heap(0xFFFFFFFF) {}
};

static PerformanceMetrics perf_metrics;

/**
 * @brief Log stability metrics (Task 11.1)
 * Called periodically to monitor system health
 */
static void log_stability_metrics() {
    uint32_t free_heap = ESP.getFreeHeap();
    int rssi = net_wifi_rssi();
    
    // Track minimum heap ever seen
    if (free_heap < perf_metrics.min_free_heap) {
        perf_metrics.min_free_heap = free_heap;
    }
    
    Serial.println("========== STABILITY METRICS ==========");
    Serial.printf("[STABILITY] Free heap: %u bytes (min: %u)\n", free_heap, perf_metrics.min_free_heap);
    Serial.printf("[STABILITY] Wi-Fi RSSI: %d dBm\n", rssi);
    Serial.printf("[STABILITY] Last price fetch: %lu ms\n", perf_metrics.last_price_fetch_duration_ms);
    Serial.printf("[STABILITY] Last funding fetch: %lu ms\n", perf_metrics.last_funding_fetch_duration_ms);
    Serial.printf("[STABILITY] Uptime: %lu seconds\n", millis() / 1000);
    Serial.println("======================================");
}

/**
 * @brief Fetch and update spot prices for all symbols
 * @return Number of successful fetches
 */
static int fetch_all_prices() {
    unsigned long fetch_start = millis();
    const AppConfig& cfg = config_get();
    int success_count = 0;
    unsigned long now = millis();
    
    for (int i = 0; i < cfg.num_symbols; i++) {
        // Check if we should retry this symbol (backoff)
        if (!price_backoff[i].should_retry(now)) {
            continue;
        }
        
        const SymbolConfig* sym = config_get_symbol(i);
        if (!sym) continue;
        
        price_backoff[i].mark_attempt(now);
        
        // Get current state to preserve other fields
        AppState snapshot = model_snapshot();
        SymbolState state = snapshot.symbols[i];
        
        state.symbol_name = sym->display_name;
        state.binance_symbol = sym->binance_symbol;
        state.coinbase_product = sym->coinbase_product;
        
        bool binance_ok = false;
        bool coinbase_ok = false;
        
        // Fetch Binance spot price
        double binance_price = 0.0;
        if (net_binance::fetch_spot(sym->binance_symbol, &binance_price)) {
            state.binance_quote.price = binance_price;
            state.binance_quote.valid = true;
            state.binance_quote.last_update_ms = millis();
            binance_ok = true;
        } else {
            state.binance_quote.valid = false;
        }
        
        // Fetch Coinbase spot price
        double coinbase_price = 0.0;
        if (net_coinbase::fetch_spot(sym->coinbase_product, &coinbase_price)) {
            state.coinbase_quote.price = coinbase_price;
            state.coinbase_quote.valid = true;
            state.coinbase_quote.last_update_ms = millis();
            coinbase_ok = true;
        } else {
            state.coinbase_quote.valid = false;
        }
        
        // Calculate spread if both prices are valid
        if (state.binance_quote.valid && state.coinbase_quote.valid) {
            double spread_abs, spread_pct;
            if (calc_spread(state.binance_quote.price, state.coinbase_quote.price, 
                          &spread_abs, &spread_pct)) {
                state.spread_abs = spread_abs;
                state.spread_pct = spread_pct;
                state.spread_valid = true;
            } else {
                state.spread_valid = false;
            }
        } else {
            state.spread_valid = false;
        }
        
        // Update timestamp if at least one quote is valid (Task 8.2)
        if (binance_ok || coinbase_ok) {
            state.last_update_ms = millis();
        }
        
        // Update model with fetched data
        model_update_symbol(i, state);
        
        // Update backoff: reset on success, increase on failure
        if (binance_ok && coinbase_ok) {
            price_backoff[i].reset();
            success_count++;
        } else {
            price_backoff[i].increase();
            Serial.printf("[SCHEDULER] Price fetch failed for %s, backing off to %lums\n",
                         sym->display_name, price_backoff[i].current_delay_ms);
        }
    }
    
    // Track fetch duration (Task 11.1)
    perf_metrics.last_price_fetch_duration_ms = millis() - fetch_start;
    
    return success_count;
}

/**
 * @brief Fetch and update funding rates for all symbols
 * @return Number of successful fetches
 */
static int fetch_all_funding() {
    unsigned long fetch_start = millis();
    const AppConfig& cfg = config_get();
    int success_count = 0;
    unsigned long now = millis();
    
    for (int i = 0; i < cfg.num_symbols; i++) {
        // Check if we should retry this symbol (backoff)
        if (!funding_backoff[i].should_retry(now)) {
            continue;
        }
        
        const SymbolConfig* sym = config_get_symbol(i);
        if (!sym) continue;
        
        funding_backoff[i].mark_attempt(now);
        
        // Get current state to preserve other fields
        AppState snapshot = model_snapshot();
        SymbolState state = snapshot.symbols[i];
        
        // Fetch Binance funding rate
        double funding_rate = 0.0;
        if (net_binance::fetch_funding(sym->binance_symbol, &funding_rate)) {
            state.funding.rate = funding_rate;
            state.funding.valid = true;
            state.funding.last_update_ms = millis();
            funding_backoff[i].reset();
            success_count++;
        } else {
            state.funding.valid = false;
            funding_backoff[i].increase();
            Serial.printf("[SCHEDULER] Funding fetch failed for %s, backing off to %lums\n",
                         sym->display_name, funding_backoff[i].current_delay_ms);
        }
        
        // Update model with funding data
        model_update_symbol(i, state);
    }
    
    // Track fetch duration (Task 11.1)
    perf_metrics.last_funding_fetch_duration_ms = millis() - fetch_start;
    
    return success_count;
}

/**
 * @brief Network task - periodic data fetching
 * 
 * Runs independently from UI loop. Fetches:
 * - Spot prices every PRICE_REFRESH_MS
 * - Funding rates every FUNDING_REFRESH_MS
 * 
 * Implements exponential backoff on failures per symbol.
 */
static void net_task(void* parameter) {
    Serial.println("[SCHEDULER] Net task started");
    
    unsigned long last_price_fetch = 0;
    unsigned long last_funding_fetch = 0;
    
    // Wait for Wi-Fi to connect before starting
    while (!net_wifi_is_connected()) {
        Serial.println("[SCHEDULER] Waiting for Wi-Fi connection...");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    
    Serial.println("[SCHEDULER] Wi-Fi connected, starting periodic fetches");
    
    while (true) {
        unsigned long now = millis();
        
        // Only fetch if Wi-Fi is connected
        if (net_wifi_is_connected()) {
            // Fetch prices based on configured interval
            if (now - last_price_fetch >= config_get_price_refresh_ms()) {
                last_price_fetch = now;
                Serial.println("[SCHEDULER] Fetching prices...");
                int success = fetch_all_prices();
                Serial.printf("[SCHEDULER] Price fetch: %d/%d successful\n", 
                            success, config_get_num_symbols());
                
                // Mark data as fresh if at least one fetch succeeded
                if (success > 0) {
                    model_set_stale(false);
                }
            }
            
            // Fetch funding rates based on configured interval
            if (now - last_funding_fetch >= config_get_funding_refresh_ms()) {
                last_funding_fetch = now;
                Serial.println("[SCHEDULER] Fetching funding rates...");
                int success = fetch_all_funding();
                Serial.printf("[SCHEDULER] Funding fetch: %d/%d successful\n",
                            success, config_get_num_symbols());
            }
        } else {
            Serial.println("[SCHEDULER] Wi-Fi disconnected, skipping fetch");
        }
        
        // Stale data detection (Task 8.2)
        // Check if any symbol data is older than STALE_MS threshold
        AppState snapshot = model_snapshot();
        bool any_stale = false;
        uint32_t stale_threshold_ms = config_get_stale_ms();
        
        for (int i = 0; i < config_get_num_symbols(); i++) {
            if (snapshot.symbols[i].last_update_ms == 0) {
                // Never updated - consider stale (but don't log every time)
                any_stale = true;
            } else {
                // Check age, handling millis() rollover correctly
                unsigned long age_ms = now - snapshot.symbols[i].last_update_ms;
                // Only log and mark stale if age exceeds threshold
                if (age_ms > stale_threshold_ms && age_ms < 4000000000UL) {
                    // Updated but too old (and not wrapped around)
                    any_stale = true;
                    Serial.printf("[SCHEDULER] %s data is stale (age: %lu ms)\n",
                                 snapshot.symbols[i].symbol_name, age_ms);
                }
            }
        }
        
        if (any_stale && !snapshot.data_stale) {
            Serial.println("[SCHEDULER] Marking data as STALE");
            model_set_stale(true);
        }
        
        // Yield to other tasks - check every second
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void scheduler_init() {
    Serial.println("[SCHEDULER] Initializing task scheduler...");
    
    // Initialize storage subsystem (Task 10.1)
    hw_storage_init();
    
    // Load configuration from storage (Task 10.1)
    // If no saved config exists, will fall back to defaults already set by config_init()
    config_load();
    
    // Initialize hardware alert module (Task 9.1)
    hw_alert_init();
    
    // Initialize alert engine (Task 9.1)
    alerts_init();
    
    // Create network task with reasonable stack size
    // Priority 1 = lower than default (UI should have priority)
    BaseType_t result = xTaskCreate(
        net_task,
        "net_task",
        8192,  // 8KB stack
        NULL,
        1,     // Low priority (UI is higher)
        &net_task_handle
    );
    
    if (result != pdPASS) {
        Serial.println("[SCHEDULER] ERROR: Failed to create net_task!");
    } else {
        Serial.println("[SCHEDULER] Net task created successfully");
    }
    
    // Create alert monitoring task (Task 9.1)
    // Priority 1 = same as net_task
    result = xTaskCreate(
        alert_task,
        "alert_task",
        4096,  // 4KB stack (less than net_task, no HTTP)
        NULL,
        1,     // Low priority
        &alert_task_handle
    );
    
    if (result != pdPASS) {
        Serial.println("[SCHEDULER] ERROR: Failed to create alert_task!");
    } else {
        Serial.println("[SCHEDULER] Alert task created successfully");
    }
}

void scheduler_stop() {
    if (net_task_handle != NULL) {
        vTaskDelete(net_task_handle);
        net_task_handle = NULL;
        Serial.println("[SCHEDULER] Net task stopped");
    }
    
    if (alert_task_handle != NULL) {
        vTaskDelete(alert_task_handle);
        alert_task_handle = NULL;
        Serial.println("[SCHEDULER] Alert task stopped");
    }
}
