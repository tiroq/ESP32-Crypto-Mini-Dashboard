# ESP32-2432S028 Crypto Mini-Dashboard

A real-time cryptocurrency price monitoring dashboard for the ESP32-2432S028 (Cheap Yellow Display) development board. Displays Binance and Coinbase spot prices with spread analysis and funding rate tracking.

<img src="images/esp32_2432S028.jpg" width="320">

Dashboard:

![Dashboard Screenshot](images/dashboard.bmp)

*Live screenshot captured via serial command showing BTC/ETH/SOL prices with funding rates*

## Hardware

- **Board**: ESP32-2432S028 (CYD - Cheap Yellow Display)
- **Display**: 2.8" ILI9341 TFT (320x240)
- **Touch**: XPT2046 resistive touchscreen
- **Connectivity**: Wi-Fi (ESP32 built-in)

## Features

- Real-time price tracking from Binance and Coinbase
- Spread calculation (absolute and percentage)
- Funding rate monitoring (Binance perpetual futures)
- Multi-symbol support (BTC, ETH, SOL)
- Touch navigation between symbols and screens
- Configurable alert thresholds
- Persistent settings (NVS storage)
- **Screenshot capture** - Capture UI via serial commands and download as BMP files

## Screenshots

The dashboard supports software screenshots via serial commands. Capture the current UI state and download it to your computer:

```bash
# Take screenshot and download in one command
python3 download_screenshot.py -s
```

**Serial Commands:**
- `SCREENSHOT` - Capture current display to SPIFFS
- `DOWNLOAD` - Download screenshot via hex-encoded serial transfer
- `LIST` - Show files on SPIFFS

The screenshot feature captures the full 320×240 display in 24-bit BMP format (~230 KB). The capture process:
1. Pauses network tasks to prevent interference
2. Captures screen in 6 passes of 40 rows each (works within available heap)
3. Intercepts LVGL's display flush callback for pixel data
4. Converts RGB565 → RGB888 and writes BMP format
5. Downloads via serial using hex encoding for reliability

See [docs/SCREENSHOT.md](docs/SCREENSHOT.md) for complete documentation.

## Configuration

### Wi-Fi Credentials

Create or edit `src/secrets.h` with your Wi-Fi credentials:

```cpp
#ifndef SECRETS_H
#define SECRETS_H

// Wi-Fi credentials - KEEP THIS FILE GITIGNORED!
#define WIFI_SSID "YourWiFiNetworkName"
#define WIFI_PASS "YourWiFiPassword"

#endif // SECRETS_H
```

**Important:** Add `src/secrets.h` to your `.gitignore` to avoid committing credentials:

```bash
echo "src/secrets.h" >> .gitignore
```

### Settings

Default configuration can be modified in `src/app/app_config.cpp`:

- **Price Refresh**: 5 seconds (5000ms)
- **Funding Refresh**: 60 seconds (60000ms)
- **Spread Alert**: 0.5% threshold
- **Funding Alert**: 0.01% threshold
- **Stale Data**: 15 seconds timeout

Settings can also be adjusted via the Settings screen on the device (persists across reboots).

## Build & Flash

Using PlatformIO:

```bash
# Build
pio run

# Upload to device
pio run -t upload --upload-port /dev/cu.usbserial-XXXXX

# Monitor serial output
pio device monitor --baud 115200
```

## Project Structure

```
/src
  main.cpp
  secrets.h          # Wi-Fi credentials (gitignored)
  app/               # Application logic
    app_model.h/.cpp       # Thread-safe state management
    app_config.h/.cpp      # Configuration defaults
    app_math.h/.cpp        # Spread calculations
    app_scheduler.h/.cpp   # FreeRTOS task management
  net/               # Networking layer
    net_wifi.h/.cpp        # Wi-Fi connection management
    net_http.h/.cpp        # HTTP client wrapper
    net_binance.h/.cpp     # Binance API adapter
    net_coinbase.h/.cpp    # Coinbase API adapter
    net_time.h/.cpp        # NTP time sync
  ui/                # User interface
    ui_root.h/.cpp         # UI initialization
    ui_screens.h/.cpp      # Screen layouts
    ui_bindings.h/.cpp     # Model-to-UI data binding
    ui_styles.h/.cpp       # UI styling
    ui_screenshot.h/.cpp   # Screenshot capture (SPIFFS)
  hw/                # Hardware abstraction
    hw_display.h/.cpp      # Display driver (LVGL)
    hw_touch.h/.cpp        # Touch input (XPT2046)
    hw_alert.h/.cpp        # Alert/buzzer output
    hw_storage.h/.cpp      # NVS persistence
```

Capture the current UI state via serial commands:

```bash
# Take screenshot and download
python3 download_screenshot.py -s
```

Or manually via serial monitor:
```
SCREENSHOT     # Capture display
LIST           # Verify file exists
DOWNLOAD       # Transfer file (use Python script to decode)
```

**What's captured:**
- Exact 320×240 pixel representation of LVGL display
- All UI elements: price cards, funding rates, spreads, status indicators
- Current symbol, WiFi signal, timestamps
- 24-bit BMP format (230,454 bytes)

**Use cases:**
- Document UI changes during development
- Capture bugs or visual issues for debugging
- Create documentation screenshots
- Monitor display state remotely

See [docs/SCREENSHOT.md](docs/SCREENSHOT.md) for technical detail
- **Funding**: Current funding rate (Binance perpetual)

### Alerts

Visual and audio alerts trigger when:
- Spread exceeds configured threshold (default 0.5%)
- Funding rate exceeds configured threshold (default 0.01%)
- Cooldown period prevents alert spam (30 seconds per symbol)

### Screenshots

Take screenshots of the current UI programmatically:

```cpp
#include "ui/ui_screenshot.h"

// Call from anywhere after ui_screenshot_init()
if (ui_take_screenshot("/dashboard.bmp")) {
    Serial.println("Screenshot saved!");
}
```

See [docs/SCREENSHOT.md](docs/SCREENSHOT.md) for detailed documentation and [docs/screenshot_examples.cpp](docs/screenshot_examples.cpp) for usage examples.

## Development

This project follows a modular architecture with clear separation:

- **No networking in UI modules** - UI only reads from model snapshots
- **No LVGL in networking modules** - Network tasks update model via thread-safe APIs
- **FreeRTOS tasks** - Networking runs in dedicated task, UI loop remains responsive
- **Mutex-protected state** - All model access is thread-safe via `model_snapshot()`

See `COPILOT_TASKS.md` for detailed implementation roadmap.

## License

See LICENSE file for details.
