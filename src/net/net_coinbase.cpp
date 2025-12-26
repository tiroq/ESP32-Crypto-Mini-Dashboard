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
        Serial.println("[COINBASE] Invalid parameters");
        return false;
    }

    // Build URL: https://api.coinbase.com/v2/prices/BTC-USD/spot
    String url = String(COINBASE_API_BASE) + product + "/spot";
    
    Serial.print("[COINBASE] Fetching spot for ");
    Serial.print(product);
    Serial.print(" from ");
    Serial.println(url);

    // Make HTTP GET request
    String response;
    if (!http_get(url.c_str(), response, 10000)) {
        Serial.println("[COINBASE] HTTP request failed");
        return false;
    }

    // Parse JSON response
    // Expected format: {"data":{"base":"BTC","USD","amount":"43250.50"}}
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, response);
    
    if (error) {
        Serial.print("[COINBASE] JSON parse failed: ");
        Serial.println(error.c_str());
        return false;
    }

    // Validate JSON structure
    if (!doc.containsKey("data")) {
        Serial.println("[COINBASE] Missing 'data' field");
        return false;
    }

    JsonObject data = doc["data"];
    if (!data.containsKey("amount")) {
        Serial.println("[COINBASE] Missing 'amount' field");
        return false;
    }

    // Extract price as string, then convert to double
    const char* amount_str = data["amount"];
    if (!amount_str) {
        Serial.println("[COINBASE] Invalid 'amount' field");
        return false;
    }

    double price = atof(amount_str);
    
    // Validate price is positive
    if (price <= 0.0) {
        Serial.print("[COINBASE] Invalid price: ");
        Serial.println(price, 2);
        return false;
    }

    // Success
    *out_price = price;
    Serial.print("[COINBASE] SUCCESS: ");
    Serial.print(product);
    Serial.print(" = $");
    Serial.println(price, 2);
    
    return true;
}

}
