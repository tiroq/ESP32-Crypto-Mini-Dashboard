#ifndef APP_MATH_H
#define APP_MATH_H

#include <Arduino.h>

// Mathematical utilities - spread calculations, etc.
// TODO: Implement spread calculation functions

bool calc_spread(double p_binance, double p_coinbase, double* spread_abs, double* spread_pct);

#endif // APP_MATH_H
