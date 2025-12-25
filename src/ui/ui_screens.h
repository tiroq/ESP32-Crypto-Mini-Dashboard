#ifndef UI_SCREENS_H
#define UI_SCREENS_H

#include <Arduino.h>
#include <lvgl.h>

// UI screens - Dashboard, Alerts, Settings

// Widget references structure for data binding
struct DashboardWidgets {
    lv_obj_t* lbl_symbol;
    lv_obj_t* lbl_wifi;
    lv_obj_t* lbl_time;
    lv_obj_t* lbl_binance_price;
    lv_obj_t* lbl_coinbase_price;
    lv_obj_t* lbl_spread_pct;
    lv_obj_t* lbl_spread_abs;
    lv_obj_t* lbl_funding;
};

// Create and return Dashboard screen
lv_obj_t* ui_screens_create_dashboard();

// Create placeholder screens
lv_obj_t* ui_screens_create_alerts();
lv_obj_t* ui_screens_create_settings();
lv_obj_t* ui_screens_create_chart();

// Get widget references for data binding
DashboardWidgets ui_screens_get_dashboard_widgets();

#endif // UI_SCREENS_H
