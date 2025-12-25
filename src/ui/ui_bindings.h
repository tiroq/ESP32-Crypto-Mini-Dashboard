#ifndef UI_BINDINGS_H
#define UI_BINDINGS_H

#include <Arduino.h>
#include "../app/app_model.h"

// UI bindings - connects model to UI widgets (Task 4.1)
// Implements smart caching to avoid unnecessary LVGL widget updates

// Initialize bindings and start periodic update timer
void ui_bindings_init();

// Apply current model state to UI widgets
// Only updates widgets when values actually change (with epsilon for floats)
void ui_bindings_apply(const AppState& state);

#endif // UI_BINDINGS_H
