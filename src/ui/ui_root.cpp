#include "ui_root.h"
#include "ui_screens.h"

void ui_root_init() {
    Serial.println("[UI] Initializing UI root...");
    
    // Create Dashboard screen and load it
    lv_obj_t* dashboard = ui_screens_create_dashboard();
    lv_scr_load(dashboard);
    
    Serial.println("[UI] Dashboard loaded");
}
