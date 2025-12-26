#include "ui_bindings.h"
#include "ui_screens.h"
#include "../app/app_model.h"
#include "../app/app_alerts.h"
#include <lvgl.h>
#include <math.h>

// Widget references
static DashboardWidgets g_widgets;

// Cached values to prevent unnecessary updates
static struct {
    int selected_symbol_idx;
    bool wifi_connected;
    int wifi_rssi;
    char time_str[16];
    double binance_price;
    double coinbase_price;
    double spread_pct;
    double spread_abs;
    double funding_rate;
    bool data_stale;
    bool alert_active;
    bool initialized;
} g_cache = { -1, false, 0, "", -1.0, -1.0, -999.0, -999.0, -999.0, true, false, false };

// Epsilon for floating point comparison
#define FLOAT_EPSILON 0.001

// Helper: compare floats with epsilon
static bool float_changed(double old_val, double new_val) {
    return fabs(old_val - new_val) > FLOAT_EPSILON;
}

// Periodic timer callback to update UI from model
static void ui_update_timer_cb(lv_timer_t* timer) {
    // Get current model snapshot (thread-safe)
    AppState state = model_snapshot();
    
    // Apply to UI (only updates changed values)
    ui_bindings_apply(state);
}

void ui_bindings_init() {
    Serial.println("[UI_BINDINGS] Initializing...");
    
    // Get widget references from dashboard screen
    g_widgets = ui_screens_get_dashboard_widgets();
    
    // Create periodic timer to update UI every 250ms
    lv_timer_create(ui_update_timer_cb, 250, NULL);
    
    Serial.println("[UI_BINDINGS] Timer created (250ms refresh)");
}

void ui_bindings_apply(const AppState& state) {
    // Get currently selected symbol data
    int idx = state.selected_symbol_idx;
    if (idx < 0 || idx >= 3) return;
    
    const SymbolState& sym = state.symbols[idx];
    
    // === UPDATE SYMBOL NAME ===
    if (g_cache.selected_symbol_idx != idx && g_widgets.lbl_symbol) {
        lv_label_set_text(g_widgets.lbl_symbol, sym.symbol_name);
        g_cache.selected_symbol_idx = idx;
    }
    
    // === UPDATE WI-FI STATUS ===
    if (g_widgets.lbl_wifi) {
        if (g_cache.wifi_connected != state.wifi_connected || g_cache.wifi_rssi != state.wifi_rssi) {
            char buf[32];
            if (state.wifi_connected) {
                snprintf(buf, sizeof(buf), "WiFi: %d dBm", state.wifi_rssi);
            } else {
                strcpy(buf, "WiFi: --");
            }
            lv_label_set_text(g_widgets.lbl_wifi, buf);
            g_cache.wifi_connected = state.wifi_connected;
            g_cache.wifi_rssi = state.wifi_rssi;
        }
    }
    
    // === UPDATE TIME (show seconds since last update) ===
    if (g_widgets.lbl_time) {
        unsigned long now = millis();
        unsigned long last_update = state.symbols[state.selected_symbol_idx].last_update_ms;
        
        if (last_update > 0) {
            unsigned long age_s = (now - last_update) / 1000;
            char time_text[16];
            snprintf(time_text, sizeof(time_text), "Last: %lus", age_s);
            lv_label_set_text(g_widgets.lbl_time, time_text);
        } else {
            lv_label_set_text(g_widgets.lbl_time, "Last: --s");
        }
    }
    
    // === HANDLE STALE DATA ===
    if (state.data_stale) {
        // Show STALE indicator if data is stale
        if (g_cache.data_stale != state.data_stale) {
            if (g_widgets.lbl_binance_price) lv_label_set_text(g_widgets.lbl_binance_price, "STALE");
            if (g_widgets.lbl_coinbase_price) lv_label_set_text(g_widgets.lbl_coinbase_price, "STALE");
            if (g_widgets.lbl_spread_pct) lv_label_set_text(g_widgets.lbl_spread_pct, "STALE");
            if (g_widgets.lbl_spread_abs) lv_label_set_text(g_widgets.lbl_spread_abs, "STALE");
            if (g_widgets.lbl_funding) lv_label_set_text(g_widgets.lbl_funding, "STALE");
            g_cache.data_stale = true;
        }
        return; // Don't update prices while stale
    }
    
    g_cache.data_stale = false;
    
    // === ALERT VISUAL INDICATOR (Task 9.1) ===
    bool alert_active = alerts_is_active();
    if (g_cache.alert_active != alert_active) {
        if (g_widgets.lbl_symbol) {
            if (alert_active) {
                // Set red border/background to indicate alert
                lv_obj_set_style_border_color(g_widgets.lbl_symbol, lv_color_hex(0xF6465D), 0);
                lv_obj_set_style_border_width(g_widgets.lbl_symbol, 3, 0);
            } else {
                // Clear border when no alert
                lv_obj_set_style_border_width(g_widgets.lbl_symbol, 0, 0);
            }
        }
        g_cache.alert_active = alert_active;
    }
    
    // === UPDATE BINANCE PRICE ===
    if (g_widgets.lbl_binance_price && sym.binance_quote.valid) {
        if (!g_cache.initialized || float_changed(g_cache.binance_price, sym.binance_quote.price)) {
            char buf[32];
            snprintf(buf, sizeof(buf), "$%.2f", sym.binance_quote.price);
            lv_label_set_text(g_widgets.lbl_binance_price, buf);
            g_cache.binance_price = sym.binance_quote.price;
        }
    } else if (g_widgets.lbl_binance_price && !sym.binance_quote.valid) {
        lv_label_set_text(g_widgets.lbl_binance_price, "$--,---.--");
    }
    
    // === UPDATE COINBASE PRICE ===
    if (g_widgets.lbl_coinbase_price && sym.coinbase_quote.valid) {
        if (!g_cache.initialized || float_changed(g_cache.coinbase_price, sym.coinbase_quote.price)) {
            char buf[32];
            snprintf(buf, sizeof(buf), "$%.2f", sym.coinbase_quote.price);
            lv_label_set_text(g_widgets.lbl_coinbase_price, buf);
            g_cache.coinbase_price = sym.coinbase_quote.price;
        }
    } else if (g_widgets.lbl_coinbase_price && !sym.coinbase_quote.valid) {
        lv_label_set_text(g_widgets.lbl_coinbase_price, "$--,---.--");
    }
    
    // === UPDATE SPREAD % ===
    if (g_widgets.lbl_spread_pct && sym.spread_valid) {
        if (!g_cache.initialized || float_changed(g_cache.spread_pct, sym.spread_pct)) {
            char buf[32];
            snprintf(buf, sizeof(buf), "%.3f%%", sym.spread_pct);
            lv_label_set_text(g_widgets.lbl_spread_pct, buf);
            
            // Color code: green if positive, red if negative
            lv_color_t color = (sym.spread_pct >= 0) ? lv_color_hex(0x0ECB81) : lv_color_hex(0xF6465D);
            lv_obj_set_style_text_color(g_widgets.lbl_spread_pct, color, 0);
            
            g_cache.spread_pct = sym.spread_pct;
        }
    } else if (g_widgets.lbl_spread_pct && !sym.spread_valid) {
        lv_label_set_text(g_widgets.lbl_spread_pct, "-.---%");
    }
    
    // === UPDATE SPREAD $ ===
    if (g_widgets.lbl_spread_abs && sym.spread_valid) {
        if (!g_cache.initialized || float_changed(g_cache.spread_abs, sym.spread_abs)) {
            char buf[32];
            snprintf(buf, sizeof(buf), "$%.2f", sym.spread_abs);
            lv_label_set_text(g_widgets.lbl_spread_abs, buf);
            
            // Color code: green if positive, red if negative
            lv_color_t color = (sym.spread_abs >= 0) ? lv_color_hex(0x0ECB81) : lv_color_hex(0xF6465D);
            lv_obj_set_style_text_color(g_widgets.lbl_spread_abs, color, 0);
            
            g_cache.spread_abs = sym.spread_abs;
        }
    } else if (g_widgets.lbl_spread_abs && !sym.spread_valid) {
        lv_label_set_text(g_widgets.lbl_spread_abs, "$-.--");
    }
    
    // === UPDATE FUNDING RATE ===
    if (g_widgets.lbl_funding && sym.funding.valid) {
        if (!g_cache.initialized || float_changed(g_cache.funding_rate, sym.funding.rate)) {
            char buf[32];
            snprintf(buf, sizeof(buf), "%.4f%%", sym.funding.rate * 100.0); // Convert to percentage
            lv_label_set_text(g_widgets.lbl_funding, buf);
            
            // Color code: yellow for positive, red for negative
            lv_color_t color = (sym.funding.rate >= 0) ? lv_color_hex(0xF0B90B) : lv_color_hex(0xF6465D);
            lv_obj_set_style_text_color(g_widgets.lbl_funding, color, 0);
            
            g_cache.funding_rate = sym.funding.rate;
        }
    } else if (g_widgets.lbl_funding && !sym.funding.valid) {
        lv_label_set_text(g_widgets.lbl_funding, "-.-----%");
    }
    
    g_cache.initialized = true;
}
