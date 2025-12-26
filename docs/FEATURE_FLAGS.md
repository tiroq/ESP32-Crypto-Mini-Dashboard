# Feature Flags Configuration

This project supports compile-time feature flags to reduce flash and RAM usage by disabling unused functionality.

## Configuration File

Edit [`src/config.h`](src/config.h) to enable/disable features:

```cpp
// Enable OTA (Over-The-Air) firmware updates via HTTP
// Savings when disabled: ~30KB flash (WebServer + Update library + UI)
#define ENABLE_OTA 1

// Enable Serial debug output
// Savings when disabled: ~5-10KB flash (removes all Serial.print* calls)
#define ENABLE_SERIAL 1

// Enable screenshot feature (save screen to SPIFFS via serial commands)
// Savings when disabled: ~1-2KB flash
#define ENABLE_SCREENSHOT 1
```

## Flash Savings by Feature

**Measured Results:**

| Configuration | Flash Used | Flash % | Savings | RAM Used | RAM % |
|--------------|------------|---------|---------|----------|-------|
| **All enabled** (default) | 1,207,901 bytes | 92.2% | baseline | 103,848 bytes | 31.7% |
| **All disabled** | 1,132,993 bytes | 86.4% | **74,908 bytes** | 103,192 bytes | 31.5% |

**Individual Feature Savings (estimated):**

| Feature | Flash Savings | What's Removed |
|---------|--------------|----------------|
| **OTA disabled** | ~68KB | WebServer library, Update library, OTA UI screen, HTTP upload page |
| **Serial disabled** | ~6KB | All debug output (Serial.println, printf, etc.) |
| **Screenshot disabled** | ~1KB | SPIFFS screenshot code, BMP encoder, serial download commands |
| **All disabled** | **~75KB** | All of the above |

## Current Flash Usage

**With all features enabled (default):**
- Flash: 92.2% (1,207,901 / 1,310,720 bytes)
- RAM: 31.7% (103,848 bytes)
- Available flash: ~102.8KB

**With all features disabled:**
- Flash: 86.4% (1,132,993 / 1,310,720 bytes)
- RAM: 31.5% (103,192 bytes)
- Available flash: **~177.7KB** (74KB more than default)
- Enables: Larger web interface, additional widgets, more fonts

## How to Disable Features

### Example 1: Disable OTA (Free ~30KB)

```cpp
// In src/config.h
#define ENABLE_OTA 0  // Changed from 1 to 0
```

**Result:** OTA button disappears from dashboard, WebServer and Update libraries not compiled

### Example 2: Disable Serial Debug (Free ~5-10KB)

```cpp
// In src/config.h
#define ENABLE_SERIAL 0  // Changed from 1 to 0
```

**Result:** All `DEBUG_PRINTLN()` calls become no-ops, Serial.begin() not called

### Example 3: Disable Screenshot (Free ~1-2KB)

```cpp
// In src/config.h
#define ENABLE_SCREENSHOT 0  // Changed from 1 to 0
```

**Result:** SPIFFS screenshot code not compiled, serial commands disabled

### Example 4: Minimal Build (Free ~35-40KB)

```cpp
// In src/config.h
#define ENABLE_OTA 0
#define ENABLE_SERIAL 0
#define ENABLE_SCREENSHOT 0
```

**Result:** Maximum flash savings for embedded deployment

## Debug Macros

When `ENABLE_SERIAL` is enabled, use these macros instead of direct Serial calls:

```cpp
DEBUG_BEGIN(115200);           // Serial.begin(115200)
DEBUG_PRINTLN("message");      // Serial.println("message")
DEBUG_PRINT(value);            // Serial.print(value)
DEBUG_PRINTF("fmt", args);     // Serial.printf("fmt", args)
```

When `ENABLE_SERIAL = 0`, these compile to nothing `((void)0)`.

## UI Behavior Changes

### OTA Disabled
- Dashboard footer: **4 buttons** instead of 5 (Prev, Next, Chart, Settings)
- Button width: **74px** instead of 58px (auto-adjusted)
- No OTA screen or web upload interface

### OTA Enabled (Default)
- Dashboard footer: **5 buttons** (Prev, Next, Chart, OTA, Settings)
- Button width: **58px** to fit all buttons
- OTA screen shows IP address for web upload

## Build Instructions

After changing flags in `config.h`:

```bash
# Clean build (recommended after flag changes)
pio run -t clean
pio run -t upload

# Or just build
pio run -t upload
```

## Notes

- **No runtime configuration**: These are compile-time flags only
- **Clean builds**: Recommended after changing flags to avoid linking issues
- **Button layout**: Dashboard footer auto-adjusts based on OTA setting
- **Serial output**: All debug messages use `DEBUG_*` macros for clean disabling
