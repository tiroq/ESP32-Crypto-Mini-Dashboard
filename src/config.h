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
#define ENABLE_SCREENSHOT 0
// Enable HTTPS for API requests (uses WiFiClientSecure with mbedTLS)
// Cost when enabled: ~131KB flash (HTTP is default, saves space)
// Note: HTTP is less secure but functional for public API endpoints
#define ENABLE_HTTPS 0
// ============================================================================
// Serial Debug Wrapper
// ============================================================================

#if ENABLE_SERIAL
    // Serial is enabled - use normal Serial object
    #define DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
    #define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)
    #define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
    #define DEBUG_BEGIN(baud) Serial.begin(baud)
#else
    // Serial is disabled - use dummy implementations (compile to nothing)
    #define DEBUG_PRINT(...) ((void)0)
    #define DEBUG_PRINTLN(...) ((void)0)
    #define DEBUG_PRINTF(...) ((void)0)
    #define DEBUG_BEGIN(baud) ((void)0)
#endif

#endif // CONFIG_H
