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
- [ ] **Redesign skin in line with Binance colors**
  - Yellow/gold accents
  - Dark theme consistency
  - Professional crypto exchange aesthetic
- [ ] **Optimize performance and reduce latency**
  - Reduce HTTP request overhead
  - Optimize LVGL rendering
  - Improve task scheduling
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
**Current**: 89.9% used (1,178,941 / 1,310,720 bytes)  
**Available**: ~132KB remaining

**Before implementing large features (OTA, web interface, localization):**
1. Remove debug Serial.printf statements (~10-20KB)
2. Optimize LVGL fonts (~20-30KB)
3. Enable aggressive compiler optimization (~10-15KB)
4. Review unused library code (~5-10KB)

**Target**: Free up 50-100KB to enable P4 features