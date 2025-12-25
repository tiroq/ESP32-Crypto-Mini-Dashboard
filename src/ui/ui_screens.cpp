#include "ui_screens.h"
#include "../app/app_model.h"

// Screen and widget references
static lv_obj_t* screen_dashboard = NULL;
static lv_obj_t* screen_alerts = NULL;
static lv_obj_t* screen_settings = NULL;

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
        lv_scr_load(screen_alerts);
    }
}

static void btn_settings_clicked(lv_event_t* e) {
    Serial.println("[UI] Settings button clicked - switching to Settings screen");
    if (screen_settings) {
        lv_scr_load(screen_settings);
    }
}

static void btn_back_clicked(lv_event_t* e) {
    Serial.println("[UI] Back button clicked - returning to Dashboard");
    if (screen_dashboard) {
        lv_scr_load(screen_dashboard);
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
    lv_obj_set_style_bg_color(header, lv_color_hex(0x1a1a1a), 0);
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
    lv_obj_align(lbl_wifi, LV_ALIGN_TOP_MID, 0, 4);

    // Time label (right)
    lbl_time = lv_label_create(header);
    lv_label_set_text(lbl_time, "--:--");
    lv_obj_set_style_text_color(lbl_time, lv_color_hex(0x888888), 0);
    lv_obj_set_style_text_font(lbl_time, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl_time, LV_ALIGN_TOP_RIGHT, -4, 4);

    // ===== DATA CARDS SECTION (middle, 160px) =====
    lv_obj_t* data_container = lv_obj_create(screen);
    lv_obj_set_size(data_container, 320, 160);
    lv_obj_set_pos(data_container, 0, 30);
    lv_obj_set_style_bg_color(data_container, lv_color_hex(0x000000), 0);
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
    lv_obj_set_style_bg_color(footer, lv_color_hex(0x1a1a1a), 0);
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
    lv_obj_set_style_bg_color(btn_prev, lv_color_hex(0x333333), 0);
    lv_obj_add_event_cb(btn_prev, btn_prev_clicked, LV_EVENT_CLICKED, NULL);
    lv_obj_t* lbl_prev = lv_label_create(btn_prev);
    lv_label_set_text(lbl_prev, "Prev");
    lv_obj_center(lbl_prev);

    // Next button
    lv_obj_t* btn_next = lv_btn_create(footer);
    lv_obj_set_size(btn_next, btn_width, btn_height);
    lv_obj_set_pos(btn_next, 2 + btn_width + btn_gap, 4);
    lv_obj_set_style_bg_color(btn_next, lv_color_hex(0x333333), 0);
    lv_obj_add_event_cb(btn_next, btn_next_clicked, LV_EVENT_CLICKED, NULL);
    lv_obj_t* lbl_next = lv_label_create(btn_next);
    lv_label_set_text(lbl_next, "Next");
    lv_obj_center(lbl_next);

    // Alerts button
    lv_obj_t* btn_alerts = lv_btn_create(footer);
    lv_obj_set_size(btn_alerts, btn_width, btn_height);
    lv_obj_set_pos(btn_alerts, 2 + (btn_width + btn_gap) * 2, 4);
    lv_obj_set_style_bg_color(btn_alerts, lv_color_hex(0x333333), 0);
    lv_obj_add_event_cb(btn_alerts, btn_alerts_clicked, LV_EVENT_CLICKED, NULL);
    lv_obj_t* lbl_alerts = lv_label_create(btn_alerts);
    lv_label_set_text(lbl_alerts, "Alerts");
    lv_obj_center(lbl_alerts);

    // Settings button
    lv_obj_t* btn_settings = lv_btn_create(footer);
    lv_obj_set_size(btn_settings, btn_width, btn_height);
    lv_obj_set_pos(btn_settings, 2 + (btn_width + btn_gap) * 3, 4);
    lv_obj_set_style_bg_color(btn_settings, lv_color_hex(0x333333), 0);
    lv_obj_add_event_cb(btn_settings, btn_settings_clicked, LV_EVENT_CLICKED, NULL);
    lv_obj_t* lbl_settings = lv_label_create(btn_settings);
    lv_label_set_text(lbl_settings, "Setup");
    lv_obj_center(lbl_settings);

    Serial.println("[UI] Dashboard screen created");
    return screen;
}

lv_obj_t* ui_screens_create_alerts() {
    lv_obj_t* screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x000000), 0);
    
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

lv_obj_t* ui_screens_create_settings() {
    lv_obj_t* screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x000000), 0);
    
    // Store screen reference
    screen_settings = screen;
    
    lv_obj_t* label = lv_label_create(screen);
    lv_label_set_text(label, "Settings Screen\nTODO");
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
    
    Serial.println("[UI] Settings screen created");
    return screen;
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
