#include "app_math.h"
#include <math.h>

bool calc_spread(double p_binance, double p_coinbase, double* spread_abs, double* spread_pct) {
    // Validate output pointers
    if (!spread_abs || !spread_pct) {
        return false;
    }
    
    // Check for invalid inputs: NaN, infinite, zero, or negative
    if (isnan(p_binance) || isnan(p_coinbase)) {
        return false;
    }
    
    if (isinf(p_binance) || isinf(p_coinbase)) {
        return false;
    }
    
    if (p_binance <= 0.0 || p_coinbase <= 0.0) {
        return false;
    }
    
    // Calculate absolute spread
    *spread_abs = p_coinbase - p_binance;
    
    // Calculate mid-price for percentage calculation
    double mid_price = (p_binance + p_coinbase) / 2.0;
    
    // Guard against division by zero (should not happen given above checks, but be safe)
    if (mid_price <= 0.0) {
        return false;
    }
    
    // Calculate percentage spread relative to mid-price
    *spread_pct = (*spread_abs / mid_price) * 100.0;
    
    return true;
}
