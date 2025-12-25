#ifndef APP_MATH_H
#define APP_MATH_H

#include <Arduino.h>

/**
 * @file app_math.h
 * @brief Mathematical utilities for crypto calculations
 * 
 * Task 7.1 - Spread calculation with mid-price formula
 */

/**
 * @brief Calculate spread between two exchange prices
 * 
 * Computes both absolute and percentage spread using mid-price formula:
 * - spread_abs = p_coinbase - p_binance
 * - spread_pct = (spread_abs / mid_price) * 100
 * - mid_price = (p_binance + p_coinbase) / 2
 * 
 * @param p_binance Price from Binance exchange
 * @param p_coinbase Price from Coinbase exchange
 * @param spread_abs Output: absolute spread in dollars
 * @param spread_pct Output: percentage spread relative to mid-price
 * @return true if calculation successful, false if invalid inputs (zero, negative, NaN)
 * 
 * Edge cases handled:
 * - Returns false if either price is <= 0
 * - Returns false if either price is NaN or infinite
 * - Returns false if output pointers are null
 * - Guards against division by zero
 */
bool calc_spread(double p_binance, double p_coinbase, double* spread_abs, double* spread_pct);

#endif // APP_MATH_H
