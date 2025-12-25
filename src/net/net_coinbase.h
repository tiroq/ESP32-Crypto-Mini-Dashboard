#ifndef NET_COINBASE_H
#define NET_COINBASE_H

#include <Arduino.h>

/**
 * @file net_coinbase.h
 * @brief Coinbase API adapter for fetching spot prices
 * 
 * API endpoint: https://api.coinbase.com/v2/prices/{product}/spot
 * Example: https://api.coinbase.com/v2/prices/BTC-USD/spot
 * Response format: {"data":{"base":"BTC","currency":"USD","amount":"43250.50"}}
 */

namespace net_coinbase {
    /**
     * @brief Fetch spot price from Coinbase for a given product
     * 
     * @param product Product ID (e.g., "BTC-USD", "ETH-USD", "SOL-USD")
     * @param out_price Pointer to store the fetched price
     * @return true if fetch successful and price valid, false on any error
     * 
     * Validates:
     * - HTTP response status (must be 200)
     * - JSON structure (data.amount exists)
     * - Price value (must be positive)
     */
    bool fetch_spot(const char* product, double* out_price);
}

#endif // NET_COINBASE_H
