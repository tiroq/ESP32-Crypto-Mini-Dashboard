#ifndef CONFIG_H
#define CONFIG_H

// ============================================================================
// Feature Flags - Enable/Disable features to save flash/RAM
// ============================================================================

// Enable OTA (Over-The-Air) firmware updates via HTTP
// Savings when disabled: ~30KB flash (WebServer + Update library + UI)
#define ENABLE_OTA 1

// Enable Serial debug output
// Savings when disabled: ~5-10KB flash (removes all Serial.print* calls)
#define ENABLE_SERIAL 1

// Enable screenshot feature (save screen to SPIFFS via serial commands)
// Cost when enabled: ~1KB flash (disabled by default)
#define ENABLE_SCREENSHOT 1

// Enable power management (sleep modes, backlight dimming, modem sleep)
// Cost when enabled: ~2KB flash, enables battery-saving features
#define ENABLE_POWER_MANAGEMENT 1

// Enable HTTPS for API requests (uses WiFiClientSecure with mbedTLS)
// Cost when enabled: ~131KB flash (HTTP is default, saves space)
// Note: HTTP is less secure but functional for public API endpoints
// WARNING: Binance and Coinbase APIs require HTTPS (return 301 on HTTP)
#define ENABLE_HTTPS 1
// ============================================================================
// Serial Debug Wrapper
// ============================================================================

#if ENABLE_SERIAL
    // Serial is enabled - use normal Serial object
    #define DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
    #define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)
    #define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
    #define DEBUG_BEGIN(baud) Serial.begin(baud)
    
    // LOG macro with F() for flash string storage (saves RAM)
    #define LOG(msg) Serial.println(F(msg))
#else
    // Serial is disabled - use dummy implementations (compile to nothing)
    #define DEBUG_PRINT(...) ((void)0)
    #define DEBUG_PRINTLN(...) ((void)0)
    #define DEBUG_PRINTF(...) ((void)0)
    #define DEBUG_BEGIN(baud) ((void)0)
    #define LOG(msg) ((void)0)
#endif

#endif // CONFIG_H
