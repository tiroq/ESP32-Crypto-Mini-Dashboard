#include "net_binance.h"
#include "net_http.h"
#include <ArduinoJson.h>

namespace net_binance {

// Binance API base URL
static const char* BINANCE_API_BASE = "https://api.binance.com";

bool fetch_spot(const char* symbol, double* out_price) {
    if (!symbol || !out_price) {
        Serial.println("[BINANCE] ERROR: Invalid parameters");
        return false;
    }
    
    // Build URL
    char url[128];
    snprintf(url, sizeof(url), "%s/api/v3/ticker/price?symbol=%s", BINANCE_API_BASE, symbol);
    
    Serial.printf("[BINANCE] Fetching spot price for %s...\n", symbol);
    
    // Fetch data
    String response;
    if (!http_get(url, response, 10000)) {
        Serial.println("[BINANCE] HTTP request failed");
        return false;
    }
    
    // Parse JSON response: {"symbol":"BTCUSDT","price":"43250.50"}
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, response);
    
    if (error) {
        Serial.printf("[BINANCE] JSON parse error: %s\n", error.c_str());
        return false;
    }
    
    // Validate required fields
    if (!doc.containsKey("symbol") || !doc.containsKey("price")) {
        Serial.println("[BINANCE] Missing required fields in response");
        return false;
    }
    
    // Verify symbol matches
    const char* resp_symbol = doc["symbol"];
    if (strcmp(resp_symbol, symbol) != 0) {
        Serial.printf("[BINANCE] Symbol mismatch: expected %s, got %s\n", symbol, resp_symbol);
        return false;
    }
    
    // Extract price (comes as string)
    const char* price_str = doc["price"];
    if (!price_str) {
        Serial.println("[BINANCE] Price field is null");
        return false;
    }
    
    *out_price = atof(price_str);
    
    if (*out_price <= 0.0) {
        Serial.printf("[BINANCE] Invalid price: %s\n", price_str);
        return false;
    }
    
    Serial.printf("[BINANCE] %s spot price: $%.2f\n", symbol, *out_price);
    return true;
}

bool fetch_funding(const char* symbol, double* out_rate) {
    // TODO: Implement Binance funding rate fetch (Task 6.3)
    return false;
}

}
