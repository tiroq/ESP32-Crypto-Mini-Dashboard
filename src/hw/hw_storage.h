#ifndef HW_STORAGE_H
#define HW_STORAGE_H

#include <Arduino.h>

// Storage abstraction (NVS/Preferences) - Task 10.1
// Provides persistent storage for application configuration using ESP32 NVS

struct AppConfig; // Forward declaration

// Initialize storage subsystem (safe to call multiple times)
void hw_storage_init();

// Load configuration from NVS
// Returns true if config was loaded successfully
// Returns false if no saved config exists or version mismatch (caller should use defaults)
bool hw_storage_load_config(AppConfig* config);

// Save configuration to NVS
// Returns true on success, false on error
bool hw_storage_save_config(const AppConfig* config);

#endif // HW_STORAGE_H
