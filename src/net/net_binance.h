#ifndef NET_BINANCE_H
#define NET_BINANCE_H

#include <Arduino.h>

// Binance API integration (Task 6.1, 6.3)
// Fetches spot prices and funding rates from Binance REST API

namespace net_binance {
    // Fetch spot price for a symbol (e.g., "BTCUSDT")
    // Uses: https://api.binance.com/api/v3/ticker/price?symbol=BTCUSDT
    // Returns: true on success with price in out_price, false on any error
    bool fetch_spot(const char* symbol, double* out_price);
    
    // Fetch current funding rate for perpetual futures (e.g., "BTCUSDT")
    // Uses: https://fapi.binance.com/fapi/v1/fundingRate?symbol=BTCUSDT&limit=1
    // Returns: true on success with rate in out_rate, false on any error
    bool fetch_funding(const char* symbol, double* out_rate);
}

#endif // NET_BINANCE_H
