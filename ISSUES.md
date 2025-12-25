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
- [ ] **Add historical data charts for price trends**
  - Requires significant flash space
  - Line charts for price history
- [ ] **Add alerts/notifications for significant price changes**
  - Threshold-based alerts
  - Visual/audio indicators
- [ ] **Enhance UI/UX with animations and transitions**
  - Smooth screen transitions
  - Loading animations
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
**Current**: 88.0% used (1,154,037 / 1,310,720 bytes)  
**Available**: ~157KB remaining

**Recent optimizations:**
- Compiler optimization (-O2): ~26KB saved
- Debug level 0: Included in optimization savings

**For future features (OTA, web interface, localization):**
1. ~~Remove debug Serial.printf statements~~ ✅ Done via CORE_DEBUG_LEVEL=0
2. Optimize LVGL fonts (~20-30KB) - Only load needed font sizes
3. Review unused library code (~5-10KB)

**Target**: Free up another 30-50KB to enable P4 features comfortably