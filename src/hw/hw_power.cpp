#include "hw_power.h"
#include "../config.h"
#include <esp_wifi.h>
#include <esp_pm.h>
#include <esp_sleep.h>
#include <driver/gpio.h>

#if ENABLE_POWER_MANAGEMENT

// Pin definitions for ESP32-2432S028
#define TFT_BL 21  // Backlight pin (PWM capable)
#define PWM_CHANNEL 0
#define PWM_FREQ 5000
#define PWM_RESOLUTION 8  // 8-bit (0-255)

// Power management state
static PowerConfig power_config;
static PowerStats power_stats = {0, 0, 0};
static uint32_t last_wake_time = 0;

bool power_init() {
    DEBUG_PRINTLN("[POWER] Initializing power management...");
    
    // Setup PWM for backlight control
    ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(TFT_BL, PWM_CHANNEL);
    ledcWrite(PWM_CHANNEL, power_config.backlight_normal);
    DEBUG_PRINTF("[POWER] Backlight PWM configured (channel %d, freq %d Hz)\n", PWM_CHANNEL, PWM_FREQ);
    
    // Initialize wake source (GPIO for touch, timer for periodic wake)
    esp_sleep_enable_timer_wakeup(power_config.sleep_duration_ms * 1000ULL);  // Convert ms to microseconds
    DEBUG_PRINTF("[POWER] Timer wake source configured (%d ms)\n", power_config.sleep_duration_ms);
    
    // Configure power management for automatic light sleep (if supported)
    #ifdef CONFIG_PM_ENABLE
    esp_pm_config_esp32_t pm_config;
    pm_config.max_freq_mhz = 240;
    pm_config.min_freq_mhz = 80;
    pm_config.light_sleep_enable = false;  // Manual control initially
    esp_pm_configure(&pm_config);
    DEBUG_PRINTLN("[POWER] PM configured (manual sleep mode)");
    #endif
    
    power_stats.uptime_ms = millis();
    last_wake_time = millis();
    
    DEBUG_PRINTLN("[POWER] Power management initialized");
    return true;
}

void power_set_mode(PowerMode mode) {
    DEBUG_PRINTF("[POWER] Setting power mode: %d\n", mode);
    power_config.mode = mode;
    power_apply_mode();
}

PowerMode power_get_mode() {
    return power_config.mode;
}

void power_set_backlight(uint8_t brightness) {
    ledcWrite(PWM_CHANNEL, brightness);
    DEBUG_PRINTF("[POWER] Backlight set to %d/255\n", brightness);
}

uint8_t power_get_backlight() {
    return ledcRead(PWM_CHANNEL);
}

void power_enable_modem_sleep() {
    DEBUG_PRINTLN("[POWER] Enabling WiFi modem sleep...");
    esp_wifi_set_ps(WIFI_PS_MIN_MODEM);  // Minimum modem sleep (better latency than MAX_MODEM)
    DEBUG_PRINTLN("[POWER] WiFi modem sleep enabled (MIN_MODEM)");
}

void power_disable_modem_sleep() {
    DEBUG_PRINTLN("[POWER] Disabling WiFi modem sleep...");
    esp_wifi_set_ps(WIFI_PS_NONE);
    DEBUG_PRINTLN("[POWER] WiFi modem sleep disabled");
}

void power_light_sleep(uint32_t duration_ms) {
    DEBUG_PRINTF("[POWER] Entering light sleep for %d ms...\n", duration_ms);
    
    // Update stats
    uint32_t wake_duration = millis() - last_wake_time;
    power_stats.uptime_ms += wake_duration;
    
    // Configure timer wake
    esp_sleep_enable_timer_wakeup(duration_ms * 1000ULL);
    
    // Enter light sleep
    esp_light_sleep_start();
    
    // Update stats after wake
    power_stats.sleep_time_ms += duration_ms;
    power_stats.wake_count++;
    last_wake_time = millis();
    
    DEBUG_PRINTF("[POWER] Woke from light sleep (wake count: %d)\n", power_stats.wake_count);
}

void power_deep_sleep(uint32_t duration_ms) {
    DEBUG_PRINTF("[POWER] Entering deep sleep for %d ms...\n", duration_ms);
    DEBUG_PRINTLN("[POWER] Device will restart on wake!");
    
    // Update stats before sleep
    uint32_t wake_duration = millis() - last_wake_time;
    power_stats.uptime_ms += wake_duration;
    power_stats.sleep_time_ms += duration_ms;
    
    // Turn off backlight
    ledcWrite(PWM_CHANNEL, 0);
    
    // Configure timer wake
    esp_sleep_enable_timer_wakeup(duration_ms * 1000ULL);
    
    // Flush serial before sleep
    DEBUG_PRINTLN("[POWER] Goodbye!");
    Serial.flush();
    delay(100);
    
    // Enter deep sleep (will restart on wake)
    esp_deep_sleep_start();
}

void power_apply_mode() {
    DEBUG_PRINTF("[POWER] Applying power mode: %d\n", power_config.mode);
    
    switch (power_config.mode) {
        case POWER_NORMAL:
            power_disable_modem_sleep();
            power_set_backlight(power_config.backlight_normal);
            DEBUG_PRINTLN("[POWER] Normal mode: Full performance");
            break;
            
        case POWER_BATTERY_SAVER:
            power_enable_modem_sleep();
            power_set_backlight(power_config.backlight_dimmed);
            DEBUG_PRINTLN("[POWER] Battery saver mode: Modem sleep + dimmed backlight");
            break;
            
        case POWER_DEEP_SLEEP:
            // Deep sleep is triggered manually by scheduler, not continuously
            power_set_backlight(power_config.backlight_dimmed);
            DEBUG_PRINTLN("[POWER] Deep sleep mode: Will sleep between updates");
            break;
            
        default:
            DEBUG_PRINTF("[POWER] ERROR: Unknown power mode %d\n", power_config.mode);
            break;
    }
}

PowerStats power_get_stats() {
    // Update current uptime
    power_stats.uptime_ms += (millis() - last_wake_time);
    last_wake_time = millis();
    return power_stats;
}

#else
// Stubs when power management is disabled

bool power_init() { return true; }
void power_set_mode(PowerMode mode) { (void)mode; }
PowerMode power_get_mode() { return POWER_NORMAL; }
void power_set_backlight(uint8_t brightness) { (void)brightness; }
uint8_t power_get_backlight() { return 255; }
void power_enable_modem_sleep() {}
void power_disable_modem_sleep() {}
void power_light_sleep(uint32_t duration_ms) { delay(duration_ms); }
void power_deep_sleep(uint32_t duration_ms) { (void)duration_ms; }
void power_apply_mode() {}
PowerStats power_get_stats() { return {0, 0, 0}; }

#endif // ENABLE_POWER_MANAGEMENT
