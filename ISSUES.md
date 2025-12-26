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
- [ ] **Add OTA updates**
  - Requires ~100KB flash space
  - Web-based firmware upload
- [ ] **Implement a web interface for remote monitoring and configuration**
  - Requires ~150KB+ flash space
  - WebSocket real-time updates
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
**Current**: 88.3% used (~1,157KB / 1,310KB)  
**Available**: ~154KB remaining

**Recent optimizations:**
- Compiler optimization (-O2): ~26KB saved
- Debug level 0: Included in optimization savings

**Recent additions:**
- Historical charts: +1,764 bytes
- Screenshot feature: +1,152 bytes
- **Total new features**: ~2.9KB

**For future features (OTA, web interface, localization):**
1. ~~Remove debug Serial.printf statements~~ ✅ Done via CORE_DEBUG_LEVEL=0
2. Optimize LVGL fonts (~20-30KB) - Only load needed font sizes
3. Review unused library code (~5-10KB)
4. Fix chart rendering (empty display issue)

**Target**: Free up another 30-50KB to enable P4 features comfortably

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