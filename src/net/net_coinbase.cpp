#include "net_coinbase.h"
#include "../config.h"
#include "net_http.h"
#include <ArduinoJson.h>

// Coinbase API base URL - use HTTP or HTTPS based on config
#if ENABLE_HTTPS
static const char* COINBASE_API_BASE = "https://api.coinbase.com/v2/prices/";
#else
static const char* COINBASE_API_BASE = "http://api.coinbase.com/v2/prices/";
#endif

namespace net_coinbase {

bool fetch_spot(const char* product, double* out_price) {
    if (!product || !out_price) {
        DEBUG_PRINTLN("[COINBASE] Invalid parameters");
        return false;
    }

    // Build URL: https://api.coinbase.com/v2/prices/BTC-USD/spot
    String url = String(COINBASE_API_BASE) + product + "/spot";
    
    DEBUG_PRINT("[COINBASE] Fetching spot for ");
    DEBUG_PRINT(product);
    DEBUG_PRINT(" from ");
    DEBUG_PRINTLN(url);

    // Make HTTP GET request
    String response;
    if (!http_get(url.c_str(), response, 10000)) {
        DEBUG_PRINTLN("[COINBASE] HTTP request failed");
        return false;
    }

    // Parse JSON response
    // Expected format: {"data":{"base":"BTC","USD","amount":"43250.50"}}
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, response);
    
    if (error) {
        DEBUG_PRINT("[COINBASE] JSON parse failed: ");
        DEBUG_PRINTLN(error.c_str());
        return false;
    }

    // Validate JSON structure
    if (!doc.containsKey("data")) {
        DEBUG_PRINTLN("[COINBASE] Missing 'data' field");
        return false;
    }

    JsonObject data = doc["data"];
    if (!data.containsKey("amount")) {
        DEBUG_PRINTLN("[COINBASE] Missing 'amount' field");
        return false;
    }

    // Extract price as string, then convert to double
    const char* amount_str = data["amount"];
    if (!amount_str) {
        DEBUG_PRINTLN("[COINBASE] Invalid 'amount' field");
        return false;
    }

    double price = atof(amount_str);
    
    // Validate price is positive
    if (price <= 0.0) {
        DEBUG_PRINT("[COINBASE] Invalid price: ");
        DEBUG_PRINTLN(price, 2);
        return false;
    }

    // Success
    *out_price = price;
    DEBUG_PRINT("[COINBASE] SUCCESS: ");
    DEBUG_PRINT(product);
    DEBUG_PRINT(" = $");
    DEBUG_PRINTLN(price, 2);
    
    return true;
}

}
