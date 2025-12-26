# ESP32 Crypto Mini Dashboard - Issues & Features

## 🔴 Critical (P0)
- [x] **No funding displayed** - Fixed HTTP parser to handle "HTTP/1.1 200" without "OK"

## 🟠 High Priority (P1)
- [x] **Redesign Settings UI**
  - ✅ Added "Saved!" confirmation popup (green for success, red for error)
  - ✅ Added "Reset to Defaults" button (orange, restores factory settings)
  - ✅ Added section headers ("Alert Thresholds" / "Refresh Intervals" in gold)
  - ✅ Improved spacing and visual hierarchy
  - ✅ Better color coding (green for alerts, cyan for intervals)
  - ✅ Clearer labels ("Spread Alert Threshold", "Price Update Rate", etc.)
  - ✅ Animated slider changes on reset

## 🟡 Medium Priority (P2)
- [x] **Redesign skin in line with Binance colors**
  - ✅ Binance dark backgrounds (#181A20 main, #0B0E11 header/footer)
  - ✅ Binance yellow/gold accents (#F0B90B) - Settings button, section headers, values
  - ✅ Binance green (#0ECB81) for positive/buy - Save button, positive spread, success popup
  - ✅ Binance red (#F6465D) for negative/sell - Reset button, negative spread, alerts, errors
  - ✅ Professional button colors (#2B3139 for nav buttons)
  - ✅ Funding rate: Yellow (positive), Red (negative) instead of yellow/cyan
  - ✅ Consistent dark theme across all screens
- [x] **Optimize performance and reduce latency**
  - ✅ Enabled compiler optimization (-O2) - Reduced flash by 25KB
  - ✅ Disabled debug logging (CORE_DEBUG_LEVEL=0) - Smaller binaries, faster execution
  - ✅ Optimized HTTP wait loop (10ms → 1ms) - Faster API responses
  - ✅ Adjusted main loop delay (5ms → 10ms) - Reduced CPU usage  
  - ✅ UI bindings already use caching - Only updates changed values
  - ✅ LVGL refresh at 250ms - Good balance between responsiveness and performance
  - **Result: Flash 90.0% → 88.0%, saved ~26KB**
- [x] **Add more coins**
  - ✅ Support for 10 symbols (BTC, ETH, SOL, XRP, ADA, DOGE, MATIC, DOT, LINK, AVAX)
  - ✅ Configurable per-symbol enable/disable flags
  - ✅ Dynamic symbol navigation (skips disabled symbols)
  - ✅ Exchange selection per symbol (primary/secondary)
  - **Flash impact**: +~300 bytes
  - Default: 3 enabled (BTC, ETH, SOL), 7 disabled
  - Next steps: UI for watchlist management
- [ ] **Add more exchanges**
  - Kraken API client implementation
  - Multi-exchange fetching logic
  - Exchange selection per symbol (already in config)

## 🟢 Low Priority (P3)
- [x] **Add historical data charts for price trends**
  - ✅ Stores last 30 price data points in circular buffer (RAM optimized)
  - ✅ LVGL line chart with auto-scaling Y-axis
  - ✅ Binance yellow (#F0B90B) line color
  - ✅ Chart accessible via Chart button (replaced Alerts in dashboard)
  - ✅ Shows price range and current symbol
  - ✅ Displays "No data yet" message when history empty
  - **Flash impact: +1,764 bytes (88.0% → 88.2%)**
  - ⚠️ **ISSUE: Chart appears empty despite data being stored**
    - Circular buffer fills correctly (verified in serial logs)
    - Chart widget created and configured
    - Possible causes:
      - Y-axis scaling issue (all points at same value collapse to line)
      - Chart rendering order (drawn before data binding)
      - Chart coordinates outside visible area
    - **TODO**: Debug chart point rendering and axis scaling
- [x] **Add screenshot capture feature**
  - ✅ Serial command interface (SCREENSHOT/DOWNLOAD/LIST)
  - ✅ Python download script with progress tracking
  - ✅ Chunked capture (6 passes of 40 rows) - Works within heap constraints
  - ✅ LVGL flush callback interception for pixel capture
  - ✅ BMP format (24-bit RGB888, 230KB file size)
  - ✅ Hex-encoded serial transfer (reliable, corruption-resistant)
  - ✅ Automatic network pause during capture
  - ✅ SPIFFS storage with auto-cleanup
  - **Flash impact: +1,152 bytes (88.2% → 88.3%)**
- [ ] **Add alerts/notifications for significant price changes**
  - Threshold-based alerts
  - Visual/audio indicators
- [x] **Enhance UI/UX with animations and transitions**
  - ✅ Smooth screen transitions (250ms slide left/right animations)
  - ✅ Back button uses slide-right, forward navigation uses slide-left
  - ✅ Dashboard initial load with 300ms fade-in
  - **Flash impact**: Minimal (uses built-in LVGL animations)
- [ ] **Add more detailed documentation**
  - User guide
  - Hardware setup guide
  - API documentation

## ⚪ Future/Research (P4)
- [x] **Add OTA updates**
  - ✅ Basic HTTP OTA server on port 8080
  - ✅ Web-based firmware upload interface
  - ✅ OTA button in dashboard footer
  - ✅ Shows IP address and upload instructions
  - ✅ Automatic reboot after successful update
  - **Flash impact: +30,756 bytes (89.8% → 92.2%)**
  - **Trade-offs**: LVGL buffer reduced (40→20 rows), heap reduced (48KB→40KB)
- [x] **Implement a web interface for remote monitoring and configuration**
  - ✅ **Implemented!** Web dashboard with real-time monitoring
  - Flash impact: +18KB (79.8% → 81.2%)
  - Features: Real-time price/funding display, settings configuration
  - REST API: GET/POST /api/prices, /api/settings, /api/settings/reset
  - Access: http://<ESP32-IP>:8080/dashboard (when connected to WiFi)
  - Binance color scheme, responsive design, auto-refresh every 5 seconds
- [ ] **Add support for different languages (localization)**
  - Requires ~50-100KB flash space
  - Multi-language strings
- [ ] **Integrate with additional APIs for more data sources**
  - News feeds
  - Social sentiment
- [ ] **Implement dark mode for the display**
  - Already dark, but add themes
- [x] **Improve power management for battery-operated setups**
  - ✅ Power modes: Normal / Battery Saver / Deep Sleep
  - ✅ WiFi modem sleep for battery saver mode
  - ✅ Configurable backlight dimming (PWM control)
  - ✅ Deep sleep support with timer wake
  - ✅ Settings UI with dropdown selector
  - ✅ Automatic sleep cycles based on refresh rates
  - **Flash impact**: +~2KB (ENABLE_POWER_MANAGEMENT flag)
  - Modes:
    * **Normal**: Full brightness, WiFi always on, no sleep
    * **Battery Saver**: Dimmed backlight (100/255), WiFi modem sleep
    * **Deep Sleep**: Device sleeps between updates, restarts on wake
- [ ] **Implement user authentication for settings**
  - PIN/password protection
  - Secure settings storage

---

## ⚠️ Flash Space Constraint
**Current**: 81.2% used (1,064,881 / 1,310,720 bytes)  
**Previous**: 79.8% used (1,046,137 bytes) - HTTP mode default
**Available**: ~246KB remaining

**Recent changes:**
- ✅ **Web dashboard implemented** (+18KB)
  - Real-time monitoring interface with REST API
  - Settings configuration via web UI
  - Responsive design with Binance colors
  - Auto-refresh prices every 5 seconds
  - Flash impact: 79.8% → 81.2% (+18,744 bytes)
- ✅ **Animations disabled** (`LV_USE_ANIMATION 0`)
  - Flash saved: ~244 bytes (minimal - only had 6 animation calls)
  - Benefit: Instant screen transitions (faster UX)
  - Theme grow disabled: No button press animations

**Optimization history:**
- Compiler optimization (-Os + gc-sections): ~26KB saved
- Debug level 0: Included in optimization savings
- Historical charts: +1,764 bytes
- Screenshot feature: +1,152 bytes  
- Animations disabled: ~244 bytes saved
- OTA updates: +30,756 bytes

**Current Flash Usage: 81.2%** (1,064,881 / 1,310,720 bytes) = **~245.8KB available**

**Current status (HTTP default):**
- ✅ **Web interface IMPLEMENTED** (dashboard + REST API working)
- ✅ **Additional features possible** (excellent flash headroom)
- ⚠️ **HTTPS optional** (costs 131KB, would use ~212KB total)
- ✅ **Localization possible** (50-100KB still fits comfortably)
- ✅ **Feature flags available** - See [FEATURE_FLAGS.md](docs/FEATURE_FLAGS.md) for configuration

**Feature flags (measured impact):**
- **Default: HTTP mode** (82.1% flash, 234KB available)
- Enable HTTPS: **+131KB flash** (adds mbedTLS, reduces to 102KB available)
- Disable OTA: **-68KB flash** (removes WebServer, Update libs)
- Disable Serial: **-6KB flash** (removes all debug output)
- Disable Screenshot: **-1KB flash** (removes SPIFFS/BMP code)
- **Minimal build: ~72% flash** (366KB available)

**Possible future optimizations:**
1. Remove screenshot feature (~1-2KB)
2. Reduce chart history (30→20 points, ~240 bytes per symbol)
3. Disable more LVGL widgets (limited gains due to dependencies)
4. Switch to 4MB flash partition scheme (difficult, requires bootloader changes)

---

## 🐛 Active Issues

### Chart Empty Display
**Status**: Under investigation  
**Symptoms**: Chart widget shows empty despite circular buffer containing data  
**Debug steps needed**:
1. Add serial logging to show Y-axis min/max during chart update
2. Verify chart point array is populated before lv_chart_set_ext_y_array()
3. Check if chart needs lv_chart_refresh() after data update
4. Test with wider price range (artificial data spread)
5. Verify chart series object is valid

**Possible fixes**:
- Force Y-axis range manually: lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, min, max)
- Trigger redraw: lv_obj_invalidate(chart)
- Check point count matches: lv_chart_set_point_count(chart, 30)