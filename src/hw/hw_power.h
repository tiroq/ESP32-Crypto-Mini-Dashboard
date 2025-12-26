#ifndef HW_POWER_H
#define HW_POWER_H

#include <Arduino.h>

// Power management modes
enum PowerMode {
    POWER_NORMAL = 0,       // Full performance, WiFi always on, no sleep
    POWER_BATTERY_SAVER,    // Modem sleep, dimmed backlight, periodic wake
    POWER_DEEP_SLEEP        // Deep sleep between updates, display off
};

// Power management configuration
struct PowerConfig {
    PowerMode mode;
    uint8_t backlight_normal;    // 0-255, full brightness
    uint8_t backlight_dimmed;    // 0-255, battery saver brightness
    uint32_t sleep_duration_ms;  // Sleep duration for deep sleep mode
    
    PowerConfig() : mode(POWER_NORMAL),
                   backlight_normal(255),
                   backlight_dimmed(100),
                   sleep_duration_ms(60000) {}  // 1 minute default
};

// Initialize power management
bool power_init();

// Set power mode
void power_set_mode(PowerMode mode);

// Get current power mode
PowerMode power_get_mode();

// Set backlight brightness (0-255)
void power_set_backlight(uint8_t brightness);

// Get current backlight brightness
uint8_t power_get_backlight();

// Enable WiFi modem sleep (reduces power, WiFi stays connected but sleeps between activity)
void power_enable_modem_sleep();

// Disable WiFi modem sleep (full performance)
void power_disable_modem_sleep();

// Enter light sleep for specified duration (ms)
// Wakes on timer or GPIO interrupt
void power_light_sleep(uint32_t duration_ms);

// Enter deep sleep (device will restart on wake)
// Configure wake timer before calling
void power_deep_sleep(uint32_t duration_ms);

// Apply current power mode settings
void power_apply_mode();

// Power statistics
struct PowerStats {
    uint32_t uptime_ms;
    uint32_t sleep_time_ms;
    uint32_t wake_count;
};

// Get power statistics
PowerStats power_get_stats();

#endif // HW_POWER_H
