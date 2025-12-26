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
- [ ] **Add more coins**
  - Support 5-10 symbols
  - Configurable watchlist
- [ ] **Add more exchanges**
  - Kraken, Coinbase Pro
  - Exchange selection per symbol

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
- [ ] **Implement a web interface for remote monitoring and configuration**
  - ❌ **NOT FEASIBLE** - Requires ~150KB flash, only ~102KB available
  - Would need to remove screenshots (-1KB), charts (-2KB), or other features
  - Alternative: Use existing OTA page for basic monitoring
  - WebSocket real-time updates would add another ~20-30KB
- [ ] **Add support for different languages (localization)**
  - Requires ~50-100KB flash space
  - Multi-language strings
- [ ] **Integrate with additional APIs for more data sources**
  - News feeds
  - Social sentiment
- [ ] **Implement dark mode for the display**
  - Already dark, but add themes
- [ ] **Improve power management for battery-operated setups**
  - Sleep modes
  - Configurable refresh rates
- [ ] **Implement user authentication for settings**
  - PIN/password protection
  - Secure settings storage

---

## ⚠️ Flash Space Constraint
**Current**: 92.2% used (1,208,145 / 1,310,720 bytes)  
**Previous**: 89.8% used (1,177,389 bytes)
**Available**: ~102KB remaining

**Recent changes:**
- ✅ **OTA updates implemented** (+30,756 bytes)
  - WebServer library (~25KB)
  - Update library (~3KB)  
  - OTA screen and HTML page (~2KB)
  - Reduced LVGL buffer (40→20 rows, saved ~12.8KB)
  - Reduced LVGL heap (48KB→40KB, saved ~8KB)
  - Net impact: +30,756 bytes
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

**Current Flash Usage: 92.2%** (1,207,901 / 1,310,720 bytes) = **~102.8KB available**

**Current limitations:**
- ❌ **Web interface NOT feasible** (~150KB needed, only 102KB available)
- ❌ **Localization NOT feasible** (~50-100KB needed)
- ✅ Small features still possible (<10KB each)

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