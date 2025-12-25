#include "net_binance.h"
#include "net_http.h"
#include <ArduinoJson.h>

namespace net_binance {

// Binance API base URLs
static const char* BINANCE_API_BASE = "https://api.binance.com";
static const char* BINANCE_FAPI_BASE = "https://fapi.binance.com";

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
    if (!symbol || !out_rate) {
        Serial.println("[BINANCE] ERROR: Invalid parameters for funding rate");
        return false;
    }
    
    // Build URL for futures API
    char url[128];
    snprintf(url, sizeof(url), "%s/fapi/v1/fundingRate?symbol=%s&limit=1", BINANCE_FAPI_BASE, symbol);
    
    Serial.printf("[BINANCE] Fetching funding rate for %s...\n", symbol);
    
    // Fetch data
    String response;
    if (!http_get(url, response, 10000)) {
        Serial.println("[BINANCE] HTTP request failed");
        return false;
    }
    
    // Parse JSON response: [{"symbol":"BTCUSDT","fundingRate":"0.00010000","fundingTime":1609459200000}]
    // Response is an array with most recent funding rate first
    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, response);
    
    if (error) {
        Serial.printf("[BINANCE] JSON parse error: %s\n", error.c_str());
        return false;
    }
    
    // Check if response is an array
    if (!doc.is<JsonArray>()) {
        Serial.println("[BINANCE] Response is not an array");
        return false;
    }
    
    JsonArray array = doc.as<JsonArray>();
    
    // Check if array has at least one element
    if (array.size() == 0) {
        Serial.println("[BINANCE] Empty funding rate array");
        return false;
    }
    
    // Get first element
    JsonObject item = array[0];
    
    // Validate required fields
    if (!item.containsKey("symbol") || !item.containsKey("fundingRate")) {
        Serial.println("[BINANCE] Missing required fields in funding rate response");
        return false;
    }
    
    // Verify symbol matches
    const char* resp_symbol = item["symbol"];
    if (strcmp(resp_symbol, symbol) != 0) {
        Serial.printf("[BINANCE] Symbol mismatch: expected %s, got %s\n", symbol, resp_symbol);
        return false;
    }
    
    // Extract funding rate (comes as string)
    const char* rate_str = item["fundingRate"];
    if (!rate_str) {
        Serial.println("[BINANCE] Funding rate field is null");
        return false;
    }
    
    *out_rate = atof(rate_str);
    
    // Funding rate can be negative, so we don't check for <= 0
    // Just validate it's a reasonable value (typically between -1% and +1%)
    if (*out_rate < -0.01 || *out_rate > 0.01) {
        Serial.printf("[BINANCE] WARNING: Unusual funding rate: %s (%.6f)\n", rate_str, *out_rate);
        // Still return true as this is a valid (if unusual) rate
    }
    
    Serial.printf("[BINANCE] %s funding rate: %.4f%% (%s)\n", symbol, *out_rate * 100.0, rate_str);
    return true;
}

}
