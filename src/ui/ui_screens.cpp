#include "ui_screens.h"
#include "../app/app_model.h"
#include "../app/app_config.h"

// Screen and widget references
static lv_obj_t* screen_dashboard = NULL;
static lv_obj_t* screen_alerts = NULL;
static lv_obj_t* screen_settings = NULL;
static lv_obj_t* screen_chart = NULL;

// Dashboard widget references (exposed for ui_bindings)
static lv_obj_t* lbl_symbol = NULL;
static lv_obj_t* lbl_wifi = NULL;
static lv_obj_t* lbl_time = NULL;
static lv_obj_t* lbl_binance_price = NULL;
static lv_obj_t* lbl_coinbase_price = NULL;
static lv_obj_t* lbl_spread_pct = NULL;
static lv_obj_t* lbl_spread_abs = NULL;
static lv_obj_t* lbl_funding = NULL;

// Button event handlers
static void btn_prev_clicked(lv_event_t* e) {
    Serial.println("[UI] Prev button clicked");
    int current = model_get_selected();
    int next = (current - 1 + 3) % 3;  // Cycle: 0->2, 2->1, 1->0
    model_set_selected(next);
    
    // Update symbol label
    if (lbl_symbol) {
        lv_label_set_text(lbl_symbol, model_get_symbol_name(next));
    }
}

static void btn_next_clicked(lv_event_t* e) {
    Serial.println("[UI] Next button clicked");
    int current = model_get_selected();
    int next = (current + 1) % 3;  // Cycle: 0->1, 1->2, 2->0
    model_set_selected(next);
    
    // Update symbol label
    if (lbl_symbol) {
        lv_label_set_text(lbl_symbol, model_get_symbol_name(next));
    }
}

static void btn_alerts_clicked(lv_event_t* e) {
    Serial.println("[UI] Alerts button clicked - switching to Alerts screen");
    if (screen_alerts) {
        lv_scr_load_anim(screen_alerts, LV_SCR_LOAD_ANIM_OVER_LEFT, 200, 0, false);
    }
}

static void btn_chart_clicked(lv_event_t* e) {
    Serial.println("[UI] Chart button clicked - switching to Chart screen");
    
    // Delete old chart screen and recreate with fresh data
    if (screen_chart) {
        lv_obj_del(screen_chart);
        screen_chart = NULL;
    }
    
    ui_screens_create_chart();
    
    if (screen_chart) {
        Serial.println("[UI] Loading chart screen...");
        lv_scr_load_anim(screen_chart, LV_SCR_LOAD_ANIM_OVER_LEFT, 200, 0, false);
    } else {
        Serial.println("[UI] ERROR: screen_chart is NULL!");
    }
}

static void btn_settings_clicked(lv_event_t* e) {
    Serial.println("[UI] Settings button clicked - switching to Settings screen");
    if (screen_settings) {
        lv_scr_load_anim(screen_settings, LV_SCR_LOAD_ANIM_OVER_LEFT, 200, 0, false);
    }
}

static void btn_back_clicked(lv_event_t* e) {
    Serial.println("[UI] Back button clicked - returning to Dashboard");
    if (screen_dashboard) {
        lv_scr_load_anim(screen_dashboard, LV_SCR_LOAD_ANIM_OVER_RIGHT, 200, 0, false);
    }
}

lv_obj_t* ui_screens_create_dashboard() {
    lv_obj_t* screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x000000), 0);
    
    // Store screen reference
    screen_dashboard = screen;

    // ===== HEADER SECTION (top 30px) =====
    lv_obj_t* header = lv_obj_create(screen);
    lv_obj_set_size(header, 320, 30);
    lv_obj_set_pos(header, 0, 0);
    lv_obj_set_style_bg_color(header, lv_color_hex(0x0B0E11), 0);
    lv_obj_set_style_border_width(header, 0, 0);
    lv_obj_set_style_pad_all(header, 4, 0);
    lv_obj_clear_flag(header, LV_OBJ_FLAG_SCROLLABLE);

    // Symbol label (left) - store reference for updates
    lbl_symbol = lv_label_create(header);
    lv_label_set_text(lbl_symbol, model_get_symbol_name(model_get_selected()));
    lv_obj_set_style_text_color(lbl_symbol, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(lbl_symbol, &lv_font_montserrat_14, 0);
    lv_obj_set_pos(lbl_symbol, 4, 4);

    // WiFi status (center)
    lbl_wifi = lv_label_create(header);
    lv_label_set_text(lbl_wifi, "WiFi: --");
    lv_obj_set_style_text_color(lbl_wifi, lv_color_hex(0x888888), 0);
    lv_obj_set_style_text_font(lbl_wifi, &lv_font_montserrat_14, 0);
    lv_obj_set_pos(lbl_wifi, 90, 4);

    // Last update time (right) - shows seconds since update
    lbl_time = lv_label_create(header);
    lv_label_set_text(lbl_time, "Last: --s");
    lv_obj_set_style_text_color(lbl_time, lv_color_hex(0x888888), 0);
    lv_obj_set_style_text_font(lbl_time, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl_time, LV_ALIGN_TOP_RIGHT, -4, 4);

    // ===== DATA CARDS SECTION (middle, 160px) =====
    lv_obj_t* data_container = lv_obj_create(screen);
    lv_obj_set_size(data_container, 320, 160);
    lv_obj_set_pos(data_container, 0, 30);
    lv_obj_set_style_bg_color(data_container, lv_color_hex(0x181A20), 0);
    lv_obj_set_style_border_width(data_container, 0, 0);
    lv_obj_set_style_pad_all(data_container, 8, 0);
    lv_obj_clear_flag(data_container, LV_OBJ_FLAG_SCROLLABLE);

    // Binance Price Card
    lv_obj_t* lbl_binance_title = lv_label_create(data_container);
    lv_label_set_text(lbl_binance_title, "Binance");
    lv_obj_set_style_text_color(lbl_binance_title, lv_color_hex(0xF3BA2F), 0);
    lv_obj_set_style_text_font(lbl_binance_title, &lv_font_montserrat_14, 0);
    lv_obj_set_pos(lbl_binance_title, 8, 4);

    lbl_binance_price = lv_label_create(data_container);
    lv_label_set_text(lbl_binance_price, "$--,---.--");
    lv_obj_set_style_text_color(lbl_binance_price, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(lbl_binance_price, &lv_font_montserrat_24, 0);
    lv_obj_set_pos(lbl_binance_price, 8, 22);

    // Coinbase Price Card
    lv_obj_t* lbl_coinbase_title = lv_label_create(data_container);
    lv_label_set_text(lbl_coinbase_title, "Coinbase");
    lv_obj_set_style_text_color(lbl_coinbase_title, lv_color_hex(0x0052FF), 0);
    lv_obj_set_style_text_font(lbl_coinbase_title, &lv_font_montserrat_14, 0);
    lv_obj_set_pos(lbl_coinbase_title, 170, 4);

    lbl_coinbase_price = lv_label_create(data_container);
    lv_label_set_text(lbl_coinbase_price, "$--,---.--");
    lv_obj_set_style_text_color(lbl_coinbase_price, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(lbl_coinbase_price, &lv_font_montserrat_24, 0);
    lv_obj_set_pos(lbl_coinbase_price, 170, 22);

    // Spread % Card
    lv_obj_t* lbl_spread_pct_title = lv_label_create(data_container);
    lv_label_set_text(lbl_spread_pct_title, "Spread %");
    lv_obj_set_style_text_color(lbl_spread_pct_title, lv_color_hex(0x888888), 0);
    lv_obj_set_style_text_font(lbl_spread_pct_title, &lv_font_montserrat_14, 0);
    lv_obj_set_pos(lbl_spread_pct_title, 8, 60);

    lbl_spread_pct = lv_label_create(data_container);
    lv_label_set_text(lbl_spread_pct, "-.---%");
    lv_obj_set_style_text_color(lbl_spread_pct, lv_color_hex(0x00FF00), 0);
    lv_obj_set_style_text_font(lbl_spread_pct, &lv_font_montserrat_14, 0);
    lv_obj_set_pos(lbl_spread_pct, 8, 78);

    // Spread Abs Card
    lv_obj_t* lbl_spread_abs_title = lv_label_create(data_container);
    lv_label_set_text(lbl_spread_abs_title, "Spread $");
    lv_obj_set_style_text_color(lbl_spread_abs_title, lv_color_hex(0x888888), 0);
    lv_obj_set_style_text_font(lbl_spread_abs_title, &lv_font_montserrat_14, 0);
    lv_obj_set_pos(lbl_spread_abs_title, 120, 60);

    lbl_spread_abs = lv_label_create(data_container);
    lv_label_set_text(lbl_spread_abs, "$-.--");
    lv_obj_set_style_text_color(lbl_spread_abs, lv_color_hex(0x00FF00), 0);
    lv_obj_set_style_text_font(lbl_spread_abs, &lv_font_montserrat_14, 0);
    lv_obj_set_pos(lbl_spread_abs, 120, 78);

    // Funding % Card
    lv_obj_t* lbl_funding_title = lv_label_create(data_container);
    lv_label_set_text(lbl_funding_title, "Funding %");
    lv_obj_set_style_text_color(lbl_funding_title, lv_color_hex(0x888888), 0);
    lv_obj_set_style_text_font(lbl_funding_title, &lv_font_montserrat_14, 0);
    lv_obj_set_pos(lbl_funding_title, 8, 106);

    lbl_funding = lv_label_create(data_container);
    lv_label_set_text(lbl_funding, "-.-----%");
    lv_obj_set_style_text_color(lbl_funding, lv_color_hex(0xFFFF00), 0);
    lv_obj_set_style_text_font(lbl_funding, &lv_font_montserrat_14, 0);
    lv_obj_set_pos(lbl_funding, 8, 124);

    // ===== FOOTER BUTTONS (bottom 50px) =====
    lv_obj_t* footer = lv_obj_create(screen);
    lv_obj_set_size(footer, 320, 50);
    lv_obj_set_pos(footer, 0, 190);
    lv_obj_set_style_bg_color(footer, lv_color_hex(0x0B0E11), 0);
    lv_obj_set_style_border_width(footer, 0, 0);
    lv_obj_set_style_pad_all(footer, 4, 0);
    lv_obj_clear_flag(footer, LV_OBJ_FLAG_SCROLLABLE);

    // Button dimensions (large touch targets)
    const int btn_width = 74;
    const int btn_height = 42;
    const int btn_gap = 2;

    // Prev button
    lv_obj_t* btn_prev = lv_btn_create(footer);
    lv_obj_set_size(btn_prev, btn_width, btn_height);
    lv_obj_set_pos(btn_prev, 2, 4);
    lv_obj_set_style_bg_color(btn_prev, lv_color_hex(0x2B3139), 0);
    lv_obj_add_event_cb(btn_prev, btn_prev_clicked, LV_EVENT_CLICKED, NULL);
    lv_obj_t* lbl_prev = lv_label_create(btn_prev);
    lv_label_set_text(lbl_prev, "Prev");
    lv_obj_center(lbl_prev);

    // Next button
    lv_obj_t* btn_next = lv_btn_create(footer);
    lv_obj_set_size(btn_next, btn_width, btn_height);
    lv_obj_set_pos(btn_next, 2 + btn_width + btn_gap, 4);
    lv_obj_set_style_bg_color(btn_next, lv_color_hex(0x2B3139), 0);
    lv_obj_add_event_cb(btn_next, btn_next_clicked, LV_EVENT_CLICKED, NULL);
    lv_obj_t* lbl_next = lv_label_create(btn_next);
    lv_label_set_text(lbl_next, "Next");
    lv_obj_center(lbl_next);

    // Chart button
    lv_obj_t* btn_chart = lv_btn_create(footer);
    lv_obj_set_size(btn_chart, btn_width, btn_height);
    lv_obj_set_pos(btn_chart, 2 + (btn_width + btn_gap) * 2, 4);
    lv_obj_set_style_bg_color(btn_chart, lv_color_hex(0x2B3139), 0);
    lv_obj_add_event_cb(btn_chart, btn_chart_clicked, LV_EVENT_CLICKED, NULL);
    lv_obj_t* lbl_chart = lv_label_create(btn_chart);
    lv_label_set_text(lbl_chart, "Chart");
    lv_obj_center(lbl_chart);

    // Settings button
    lv_obj_t* btn_settings = lv_btn_create(footer);
    lv_obj_set_size(btn_settings, btn_width, btn_height);
    lv_obj_set_pos(btn_settings, 2 + (btn_width + btn_gap) * 3, 4);
    lv_obj_set_style_bg_color(btn_settings, lv_color_hex(0xF0B90B), 0);
    lv_obj_add_event_cb(btn_settings, btn_settings_clicked, LV_EVENT_CLICKED, NULL);
    lv_obj_t* lbl_settings = lv_label_create(btn_settings);
    lv_label_set_text(lbl_settings, "Setup");
    lv_obj_center(lbl_settings);

    Serial.println("[UI] Dashboard screen created");
    return screen;
}

lv_obj_t* ui_screens_create_alerts() {
    lv_obj_t* screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x181A20), 0);
    
    // Store screen reference
    screen_alerts = screen;
    
    lv_obj_t* label = lv_label_create(screen);
    lv_label_set_text(label, "Alerts Screen\nTODO");
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_center(label);
    
    // Back button
    lv_obj_t* btn_back = lv_btn_create(screen);
    lv_obj_set_size(btn_back, 100, 50);
    lv_obj_align(btn_back, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_add_event_cb(btn_back, btn_back_clicked, LV_EVENT_CLICKED, NULL);
    lv_obj_t* lbl_back = lv_label_create(btn_back);
    lv_label_set_text(lbl_back, "Back");
    lv_obj_center(lbl_back);
    
    Serial.println("[UI] Alerts screen created");
    return screen;
}

// Forward declarations for settings screen
static void settings_save_clicked(lv_event_t* e);
static void settings_reset_clicked(lv_event_t* e);
static void settings_spread_changed(lv_event_t* e);
static void settings_funding_changed(lv_event_t* e);
static void settings_price_refresh_changed(lv_event_t* e);
static void settings_funding_refresh_changed(lv_event_t* e);

// Settings screen widgets (Task 10.2)
static lv_obj_t* slider_spread = NULL;
static lv_obj_t* slider_funding = NULL;
static lv_obj_t* slider_price_refresh = NULL;
static lv_obj_t* slider_funding_refresh = NULL;
static lv_obj_t* lbl_spread_value = NULL;
static lv_obj_t* lbl_funding_value = NULL;
static lv_obj_t* lbl_price_refresh_value = NULL;
static lv_obj_t* lbl_funding_refresh_value = NULL;

lv_obj_t* ui_screens_create_settings() {
    lv_obj_t* screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x181A20), 0);
    
    // Store screen reference
    screen_settings = screen;
    
    // Top button bar with improved spacing
    lv_obj_t* btn_back = lv_btn_create(screen);
    lv_obj_set_size(btn_back, 70, 40);
    lv_obj_set_pos(btn_back, 5, 5);
    lv_obj_set_style_bg_color(btn_back, lv_color_hex(0x2B3139), 0);
    lv_obj_add_event_cb(btn_back, btn_back_clicked, LV_EVENT_CLICKED, NULL);
    lv_obj_t* lbl_back = lv_label_create(btn_back);
    lv_label_set_text(lbl_back, "Back");
    lv_obj_center(lbl_back);
    
    // Save button (green)
    lv_obj_t* btn_save = lv_btn_create(screen);
    lv_obj_set_size(btn_save, 70, 40);
    lv_obj_set_pos(btn_save, 165, 5);
    lv_obj_set_style_bg_color(btn_save, lv_color_hex(0x0ECB81), 0);
    lv_obj_add_event_cb(btn_save, settings_save_clicked, LV_EVENT_CLICKED, NULL);
    lv_obj_t* lbl_save = lv_label_create(btn_save);
    lv_label_set_text(lbl_save, "Save");
    lv_obj_center(lbl_save);
    
    // Reset button (red)
    lv_obj_t* btn_reset = lv_btn_create(screen);
    lv_obj_set_size(btn_reset, 70, 40);
    lv_obj_set_pos(btn_reset, 245, 5);
    lv_obj_set_style_bg_color(btn_reset, lv_color_hex(0xF6465D), 0);
    lv_obj_add_event_cb(btn_reset, settings_reset_clicked, LV_EVENT_CLICKED, NULL);
    lv_obj_t* lbl_reset = lv_label_create(btn_reset);
    lv_label_set_text(lbl_reset, "Reset");
    lv_obj_center(lbl_reset);
    
    // Title
    lv_obj_t* lbl_title = lv_label_create(screen);
    lv_label_set_text(lbl_title, "Settings");
    lv_obj_set_style_text_color(lbl_title, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(lbl_title, &lv_font_montserrat_14, 0);
    lv_obj_set_pos(lbl_title, 115, 55);
    
    int y_pos = 85;
    int row_height = 50;
    
    // ===== ALERT THRESHOLDS SECTION =====
    lv_obj_t* lbl_section1 = lv_label_create(screen);
    lv_label_set_text(lbl_section1, "Alert Thresholds");
    lv_obj_set_style_text_color(lbl_section1, lv_color_hex(0xF0B90B), 0);
    lv_obj_set_pos(lbl_section1, 10, y_pos);
    
    y_pos += 20;
    
    // Spread alert threshold (0.1% to 2.0%, step 0.1%)
    lv_obj_t* lbl_spread = lv_label_create(screen);
    lv_label_set_text(lbl_spread, "Spread Alert Threshold");
    lv_obj_set_style_text_color(lbl_spread, lv_color_hex(0xCCCCCC), 0);
    lv_obj_set_pos(lbl_spread, 15, y_pos);
    
    lbl_spread_value = lv_label_create(screen);
    lv_obj_set_style_text_color(lbl_spread_value, lv_color_hex(0xF0B90B), 0);
    lv_obj_set_style_text_font(lbl_spread_value, &lv_font_montserrat_14, 0);
    lv_obj_set_pos(lbl_spread_value, 260, y_pos - 2);
    
    slider_spread = lv_slider_create(screen);
    lv_obj_set_size(slider_spread, 230, 12);
    lv_obj_set_pos(slider_spread, 15, y_pos + 20);
    lv_slider_set_range(slider_spread, 1, 20); // 0.1% to 2.0% (value * 0.1)
    lv_slider_set_value(slider_spread, (int)(config_get_spread_alert_pct() * 10), LV_ANIM_OFF);
    lv_obj_add_event_cb(slider_spread, settings_spread_changed, LV_EVENT_VALUE_CHANGED, NULL);
    
    y_pos += row_height;
    
    // Funding alert threshold (0.001% to 0.05%, step 0.001%)
    lv_obj_t* lbl_funding = lv_label_create(screen);
    lv_label_set_text(lbl_funding, "Funding Rate Alert");
    lv_obj_set_style_text_color(lbl_funding, lv_color_hex(0xCCCCCC), 0);
    lv_obj_set_pos(lbl_funding, 15, y_pos);
    
    lbl_funding_value = lv_label_create(screen);
    lv_obj_set_style_text_color(lbl_funding_value, lv_color_hex(0xF0B90B), 0);
    lv_obj_set_style_text_font(lbl_funding_value, &lv_font_montserrat_14, 0);
    lv_obj_set_pos(lbl_funding_value, 260, y_pos - 2);
    
    slider_funding = lv_slider_create(screen);
    lv_obj_set_size(slider_funding, 230, 12);
    lv_obj_set_pos(slider_funding, 15, y_pos + 20);
    lv_slider_set_range(slider_funding, 1, 50); // 0.001% to 0.05% (value * 0.001)
    lv_slider_set_value(slider_funding, (int)(config_get_funding_alert_pct() * 1000), LV_ANIM_OFF);
    lv_obj_add_event_cb(slider_funding, settings_funding_changed, LV_EVENT_VALUE_CHANGED, NULL);
    
    y_pos += row_height + 10;
    
    // ===== REFRESH INTERVALS SECTION =====
    lv_obj_t* lbl_section2 = lv_label_create(screen);
    lv_label_set_text(lbl_section2, "Refresh Intervals");
    lv_obj_set_style_text_color(lbl_section2, lv_color_hex(0xF0B90B), 0);
    lv_obj_set_pos(lbl_section2, 10, y_pos);
    
    y_pos += 20;
    
    // Price refresh interval (1s to 30s)
    lv_obj_t* lbl_price_refresh = lv_label_create(screen);
    lv_label_set_text(lbl_price_refresh, "Price Update Rate");
    lv_obj_set_style_text_color(lbl_price_refresh, lv_color_hex(0xCCCCCC), 0);
    lv_obj_set_pos(lbl_price_refresh, 15, y_pos);
    
    lbl_price_refresh_value = lv_label_create(screen);
    lv_obj_set_style_text_color(lbl_price_refresh_value, lv_color_hex(0xEAECEF), 0);
    lv_obj_set_style_text_font(lbl_price_refresh_value, &lv_font_montserrat_14, 0);
    lv_obj_set_pos(lbl_price_refresh_value, 260, y_pos - 2);
    
    slider_price_refresh = lv_slider_create(screen);
    lv_obj_set_size(slider_price_refresh, 230, 12);
    lv_obj_set_pos(slider_price_refresh, 15, y_pos + 20);
    lv_slider_set_range(slider_price_refresh, 1, 30); // 1s to 30s
    lv_slider_set_value(slider_price_refresh, config_get_price_refresh_ms() / 1000, LV_ANIM_OFF);
    lv_obj_add_event_cb(slider_price_refresh, settings_price_refresh_changed, LV_EVENT_VALUE_CHANGED, NULL);
    
    y_pos += row_height;
    
    // Funding refresh interval (10s to 300s)
    lv_obj_t* lbl_funding_refresh = lv_label_create(screen);
    lv_label_set_text(lbl_funding_refresh, "Funding Rate Update");
    lv_obj_set_style_text_color(lbl_funding_refresh, lv_color_hex(0xCCCCCC), 0);
    lv_obj_set_pos(lbl_funding_refresh, 15, y_pos);
    
    lbl_funding_refresh_value = lv_label_create(screen);
    lv_obj_set_style_text_color(lbl_funding_refresh_value, lv_color_hex(0xEAECEF), 0);
    lv_obj_set_style_text_font(lbl_funding_refresh_value, &lv_font_montserrat_14, 0);
    lv_obj_set_pos(lbl_funding_refresh_value, 260, y_pos - 2);
    
    slider_funding_refresh = lv_slider_create(screen);
    lv_obj_set_size(slider_funding_refresh, 230, 12);
    lv_obj_set_pos(slider_funding_refresh, 15, y_pos + 20);
    lv_slider_set_range(slider_funding_refresh, 10, 300); // 10s to 300s
    lv_slider_set_value(slider_funding_refresh, config_get_funding_refresh_ms() / 1000, LV_ANIM_OFF);
    lv_obj_add_event_cb(slider_funding_refresh, settings_funding_refresh_changed, LV_EVENT_VALUE_CHANGED, NULL);
    
    // Update value labels
    settings_spread_changed(NULL);
    settings_funding_changed(NULL);
    settings_price_refresh_changed(NULL);
    settings_funding_refresh_changed(NULL);
    
    Serial.println("[UI] Settings screen created with controls");
    return screen;
}

// Settings slider callbacks (Task 10.2)
static void settings_spread_changed(lv_event_t* e) {
    if (slider_spread && lbl_spread_value) {
        int val = lv_slider_get_value(slider_spread);
        double pct = val * 0.1;
        char buf[16];
        snprintf(buf, sizeof(buf), "%.1f%%", pct);
        lv_label_set_text(lbl_spread_value, buf);
    }
}

static void settings_funding_changed(lv_event_t* e) {
    if (slider_funding && lbl_funding_value) {
        int val = lv_slider_get_value(slider_funding);
        double pct = val * 0.001;
        char buf[16];
        snprintf(buf, sizeof(buf), "%.3f%%", pct);
        lv_label_set_text(lbl_funding_value, buf);
    }
}

static void settings_price_refresh_changed(lv_event_t* e) {
    if (slider_price_refresh && lbl_price_refresh_value) {
        int val = lv_slider_get_value(slider_price_refresh);
        char buf[16];
        snprintf(buf, sizeof(buf), "%d s", val);
        lv_label_set_text(lbl_price_refresh_value, buf);
    }
}

static void settings_funding_refresh_changed(lv_event_t* e) {
    if (slider_funding_refresh && lbl_funding_refresh_value) {
        int val = lv_slider_get_value(slider_funding_refresh);
        char buf[16];
        snprintf(buf, sizeof(buf), "%d s", val);
        lv_label_set_text(lbl_funding_refresh_value, buf);
    }
}

static void settings_save_clicked(lv_event_t* e) {
    Serial.println("[UI] Save button clicked");
    
    // Read slider values and update config
    if (slider_spread) {
        int val = lv_slider_get_value(slider_spread);
        double pct = val * 0.1;
        config_set_spread_alert_pct(pct);
    }
    
    if (slider_funding) {
        int val = lv_slider_get_value(slider_funding);
        double pct = val * 0.001;
        config_set_funding_alert_pct(pct);
    }
    
    if (slider_price_refresh) {
        int val = lv_slider_get_value(slider_price_refresh);
        config_set_price_refresh_ms(val * 1000);
    }
    
    if (slider_funding_refresh) {
        int val = lv_slider_get_value(slider_funding_refresh);
        config_set_funding_refresh_ms(val * 1000);
    }
    
    // Save to NVS
    if (config_save()) {
        Serial.println("[UI] Settings saved successfully to NVS");
        
        // Show "Saved!" confirmation popup
        lv_obj_t* popup = lv_obj_create(lv_scr_act());
        lv_obj_set_size(popup, 160, 80);
        lv_obj_center(popup);
        lv_obj_set_style_bg_color(popup, lv_color_hex(0x0ECB81), 0);
        lv_obj_set_style_border_width(popup, 2, 0);
        lv_obj_set_style_border_color(popup, lv_color_hex(0x0ECB81), 0);
        lv_obj_set_style_radius(popup, 10, 0);
        
        lv_obj_t* lbl_popup = lv_label_create(popup);
        lv_label_set_text(lbl_popup, "Saved!");
        lv_obj_set_style_text_font(lbl_popup, &lv_font_montserrat_24, 0);
        lv_obj_set_style_text_color(lbl_popup, lv_color_hex(0xFFFFFF), 0);
        lv_obj_center(lbl_popup);
        
        lv_obj_del_delayed(popup, 1500);
    } else {
        Serial.println("[UI] ERROR: Failed to save settings");
        
        // Show error popup
        lv_obj_t* popup = lv_obj_create(lv_scr_act());
        lv_obj_set_size(popup, 160, 80);
        lv_obj_center(popup);
        lv_obj_set_style_bg_color(popup, lv_color_hex(0xF6465D), 0);
        lv_obj_set_style_border_width(popup, 2, 0);
        lv_obj_set_style_border_color(popup, lv_color_hex(0xF6465D), 0);
        lv_obj_set_style_radius(popup, 10, 0);
        
        lv_obj_t* lbl_popup = lv_label_create(popup);
        lv_label_set_text(lbl_popup, "Error!");
        lv_obj_set_style_text_font(lbl_popup, &lv_font_montserrat_24, 0);
        lv_obj_set_style_text_color(lbl_popup, lv_color_hex(0xFFFFFF), 0);
        lv_obj_center(lbl_popup);
        
        lv_obj_del_delayed(popup, 1500);
    }
}

static void settings_reset_clicked(lv_event_t* e) {
    Serial.println("[UI] Reset button clicked - restoring defaults");
    
    // Reset to default values
    config_set_spread_alert_pct(0.5);         // Default 0.5%
    config_set_funding_alert_pct(0.01);       // Default 0.01%
    config_set_price_refresh_ms(5000);        // Default 5s
    config_set_funding_refresh_ms(60000);     // Default 60s
    
    // Update sliders to reflect defaults
    if (slider_spread) {
        lv_slider_set_value(slider_spread, 5, LV_ANIM_ON);
        settings_spread_changed(NULL);
    }
    if (slider_funding) {
        lv_slider_set_value(slider_funding, 10, LV_ANIM_ON);
        settings_funding_changed(NULL);
    }
    if (slider_price_refresh) {
        lv_slider_set_value(slider_price_refresh, 5, LV_ANIM_ON);
        settings_price_refresh_changed(NULL);
    }
    if (slider_funding_refresh) {
        lv_slider_set_value(slider_funding_refresh, 60, LV_ANIM_ON);
        settings_funding_refresh_changed(NULL);
    }
    
    // Show confirmation
    lv_obj_t* popup = lv_obj_create(lv_scr_act());
    lv_obj_set_size(popup, 180, 80);
    lv_obj_center(popup);
    lv_obj_set_style_bg_color(popup, lv_color_hex(0xF0B90B), 0);
    lv_obj_set_style_border_width(popup, 2, 0);
    lv_obj_set_style_border_color(popup, lv_color_hex(0xF0B90B), 0);
    lv_obj_set_style_radius(popup, 10, 0);
    
    lv_obj_t* lbl_popup = lv_label_create(popup);
    lv_label_set_text(lbl_popup, "Reset!");
    lv_obj_set_style_text_font(lbl_popup, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(lbl_popup, lv_color_hex(0xFFFFFF), 0);
    lv_obj_center(lbl_popup);
    
    lv_obj_del_delayed(popup, 1500);
}

DashboardWidgets ui_screens_get_dashboard_widgets() {
    DashboardWidgets widgets;
    widgets.lbl_symbol = lbl_symbol;
    widgets.lbl_wifi = lbl_wifi;
    widgets.lbl_time = lbl_time;
    widgets.lbl_binance_price = lbl_binance_price;
    widgets.lbl_coinbase_price = lbl_coinbase_price;
    widgets.lbl_spread_pct = lbl_spread_pct;
    widgets.lbl_spread_abs = lbl_spread_abs;
    widgets.lbl_funding = lbl_funding;
    return widgets;
}

// Chart screen with price history
lv_obj_t* ui_screens_create_chart() {
    Serial.println("[UI] Creating chart screen...");
    
    lv_obj_t* screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x181A20), 0);
    
    // Store screen reference
    screen_chart = screen;
    
    // Back button
    lv_obj_t* btn_back = lv_btn_create(screen);
    lv_obj_set_size(btn_back, 80, 40);
    lv_obj_set_pos(btn_back, 5, 5);
    lv_obj_set_style_bg_color(btn_back, lv_color_hex(0x2B3139), 0);
    lv_obj_add_event_cb(btn_back, btn_back_clicked, LV_EVENT_CLICKED, NULL);
    lv_obj_t* lbl_back = lv_label_create(btn_back);
    lv_label_set_text(lbl_back, "Back");
    lv_obj_center(lbl_back);
    
    // Title - get current symbol
    AppState state = model_snapshot();
    const char* symbol = state.symbols[state.selected_symbol_idx].symbol_name;
    const SymbolState& sym = state.symbols[state.selected_symbol_idx];
    
    lv_obj_t* lbl_title = lv_label_create(screen);
    char title[32];
    snprintf(title, sizeof(title), "%s History", symbol);
    lv_label_set_text(lbl_title, title);
    lv_obj_set_style_text_color(lbl_title, lv_color_hex(0xF0B90B), 0);
    lv_obj_set_style_text_font(lbl_title, &lv_font_montserrat_14, 0);
    lv_obj_set_pos(lbl_title, 90, 5);
    
    // Current price (top right)
    lv_obj_t* lbl_price = lv_label_create(screen);
    char price_text[32];
    if (sym.binance_quote.valid) {
        snprintf(price_text, sizeof(price_text), "$%.2f", sym.binance_quote.price);
    } else {
        snprintf(price_text, sizeof(price_text), "---");
    }
    lv_label_set_text(lbl_price, price_text);
    lv_obj_set_style_text_color(lbl_price, lv_color_hex(0xEAECEF), 0);
    lv_obj_set_style_text_font(lbl_price, &lv_font_montserrat_14, 0);
    lv_obj_set_pos(lbl_price, 240, 5);
    
    // Create chart (fits 320x240 screen: 5px margins, title at top, range label at bottom)
    lv_obj_t* chart = lv_chart_create(screen);
    lv_obj_set_size(chart, 310, 180);  // Width: 320-10, Height: leaves room for title and range
    lv_obj_set_pos(chart, 5, 30);      // Start below title
    lv_obj_set_style_bg_color(chart, lv_color_hex(0x0B0E11), 0);
    lv_obj_set_style_border_color(chart, lv_color_hex(0x2B3139), 0);
    lv_obj_set_style_border_width(chart, 2, 0);
    
    // Configure chart
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(chart, PRICE_HISTORY_SIZE);
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 10000); // Will be auto-scaled
    
    // Add series for price data
    lv_chart_series_t* series = lv_chart_add_series(chart, lv_color_hex(0xF0B90B), LV_CHART_AXIS_PRIMARY_Y);
    
    // Populate with history data
    Serial.printf("[CHART] Drawing chart for symbol %d: history_count=%d, history_head=%d\n", 
                  state.selected_symbol_idx, sym.history_count, sym.history_head);
    
    if (sym.history_count > 0) {
        // Find min/max for auto-scaling
        double min_price = sym.price_history[0];
        double max_price = sym.price_history[0];
        
        for (int i = 0; i < sym.history_count; i++) {
            if (sym.price_history[i] < min_price) min_price = sym.price_history[i];
            if (sym.price_history[i] > max_price) max_price = sym.price_history[i];
        }
        
        // Add 5% padding
        double range = max_price - min_price;
        min_price -= range * 0.05;
        max_price += range * 0.05;
        
        Serial.printf("[CHART] Y-axis range: %.2f to %.2f (range: %.2f)\n", min_price, max_price, range);
        
        lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, (int)min_price, (int)max_price);
        
        // Populate data points
        for (int i = 0; i < PRICE_HISTORY_SIZE; i++) {
            if (i < sym.history_count) {
                int idx = (sym.history_head - sym.history_count + i + PRICE_HISTORY_SIZE) % PRICE_HISTORY_SIZE;
                series->y_points[i] = (lv_coord_t)sym.price_history[idx];
                if (i < 5 || i >= sym.history_count - 5) {
                    Serial.printf("[CHART] Point[%d] = %.2f (from history[%d])\n", 
                                  i, sym.price_history[idx], idx);
                }
            } else {
                series->y_points[i] = LV_CHART_POINT_NONE;
            }
        }
        
        lv_chart_refresh(chart);
        
        // Show price range (at bottom of screen)
        lv_obj_t* lbl_range = lv_label_create(screen);
        char range_text[64];
        snprintf(range_text, sizeof(range_text), "Range: $%.2f - $%.2f", min_price, max_price);
        lv_label_set_text(lbl_range, range_text);
        lv_obj_set_style_text_color(lbl_range, lv_color_hex(0xEAECEF), 0);
        lv_obj_set_style_text_font(lbl_range, &lv_font_montserrat_14, 0);
        lv_obj_set_pos(lbl_range, 10, 220);  // Bottom of 240px screen
    } else {
        // No data yet - centered on screen
        lv_obj_t* lbl_no_data = lv_label_create(screen);
        lv_label_set_text(lbl_no_data, "No price history yet\nData will appear after\nfetching prices");
        lv_obj_set_style_text_color(lbl_no_data, lv_color_hex(0x888888), 0);
        lv_obj_set_style_text_align(lbl_no_data, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_center(lbl_no_data);
    }
    
    Serial.println("[UI] Chart screen created");
    return screen;
}
