#include "ui_root.h"
#include "ui_screens.h"
#include "ui_bindings.h"
#include "../app/app_model.h"

void ui_root_init() {
    Serial.println("[UI] Initializing UI root...");
    
    // Initialize model
    model_init();
    
    // Create all screens
    lv_obj_t* dashboard = ui_screens_create_dashboard();
    ui_screens_create_alerts();    // Creates and stores screen_alerts
    ui_screens_create_settings();  // Creates and stores screen_settings
    ui_screens_create_chart();     // Creates and stores screen_chart
    
    // Load Dashboard with fade-in animation
    lv_scr_load_anim(dashboard, LV_SCR_LOAD_ANIM_FADE_IN, 300, 0, false);
    
    // Initialize bindings (starts periodic update timer)
    ui_bindings_init();
    
    Serial.println("[UI] Dashboard loaded and bindings initialized");
}
