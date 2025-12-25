#ifndef NET_BINANCE_H
#define NET_BINANCE_H

#include <Arduino.h>

// Binance API integration
// TODO: Implement Binance spot price and funding rate fetching

namespace net_binance {
    bool fetch_spot(const char* symbol, double* out_price);
    bool fetch_funding(const char* symbol, double* out_rate);
}

#endif // NET_BINANCE_H
