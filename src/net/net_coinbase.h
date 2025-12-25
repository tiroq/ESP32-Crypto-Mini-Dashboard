#ifndef NET_COINBASE_H
#define NET_COINBASE_H

#include <Arduino.h>

// Coinbase API integration
// TODO: Implement Coinbase spot price fetching

namespace net_coinbase {
    bool fetch_spot(const char* product, double* out_price);
}

#endif // NET_COINBASE_H
