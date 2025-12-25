#ifndef APP_MODEL_H
#define APP_MODEL_H

#include <Arduino.h>

// Application model - State management (stub for Task 2.2)

// Initialize model with default selected symbol
void model_init();

// Get currently selected symbol index (0=BTC, 1=ETH, 2=SOL)
int model_get_selected();

// Set currently selected symbol index
void model_set_selected(int idx);

// Get symbol name by index
const char* model_get_symbol_name(int idx);

#endif // APP_MODEL_H
