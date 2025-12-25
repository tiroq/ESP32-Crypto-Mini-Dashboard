/**
 * @file test_spread.cpp
 * @brief Unit tests for spread calculation (Task 7.1)
 * 
 * Tests cover:
 * - Normal calculations with valid prices
 * - Edge cases: zero, negative, NaN, infinity
 * - Null pointer handling
 * - Mid-price formula validation
 */

#include <unity.h>
#include <app/app_math.h>
#include <math.h>

// Test normal spread calculation with positive prices
void test_spread_normal_calculation() {
    double spread_abs, spread_pct;
    
    // Binance: $100, Coinbase: $102
    // Expected: abs = $2, pct = 2/101 * 100 = 1.98%
    bool result = calc_spread(100.0, 102.0, &spread_abs, &spread_pct);
    
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_FLOAT_WITHIN(0.01, 2.0, spread_abs);
    TEST_ASSERT_FLOAT_WITHIN(0.01, 1.98, spread_pct);
}

// Test negative spread (Coinbase cheaper than Binance)
void test_spread_negative() {
    double spread_abs, spread_pct;
    
    // Binance: $100, Coinbase: $98
    // Expected: abs = -$2, pct = -2/99 * 100 = -2.02%
    bool result = calc_spread(100.0, 98.0, &spread_abs, &spread_pct);
    
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_FLOAT_WITHIN(0.01, -2.0, spread_abs);
    TEST_ASSERT_FLOAT_WITHIN(0.01, -2.02, spread_pct);
}

// Test with equal prices (zero spread)
void test_spread_zero() {
    double spread_abs, spread_pct;
    
    // Both exchanges at $100
    bool result = calc_spread(100.0, 100.0, &spread_abs, &spread_pct);
    
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_FLOAT_WITHIN(0.001, 0.0, spread_abs);
    TEST_ASSERT_FLOAT_WITHIN(0.001, 0.0, spread_pct);
}

// Test with large price values (BTC-level)
void test_spread_large_values() {
    double spread_abs, spread_pct;
    
    // Binance: $87,478.90, Coinbase: $87,409.70
    // Expected: abs = -$69.20, pct ≈ -0.079%
    bool result = calc_spread(87478.90, 87409.70, &spread_abs, &spread_pct);
    
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_FLOAT_WITHIN(0.1, -69.20, spread_abs);
    TEST_ASSERT_FLOAT_WITHIN(0.001, -0.079, spread_pct);
}

// Test rejection of zero Binance price
void test_spread_zero_binance_price() {
    double spread_abs, spread_pct;
    
    bool result = calc_spread(0.0, 100.0, &spread_abs, &spread_pct);
    
    TEST_ASSERT_FALSE(result);
}

// Test rejection of zero Coinbase price
void test_spread_zero_coinbase_price() {
    double spread_abs, spread_pct;
    
    bool result = calc_spread(100.0, 0.0, &spread_abs, &spread_pct);
    
    TEST_ASSERT_FALSE(result);
}

// Test rejection of negative Binance price
void test_spread_negative_binance_price() {
    double spread_abs, spread_pct;
    
    bool result = calc_spread(-100.0, 102.0, &spread_abs, &spread_pct);
    
    TEST_ASSERT_FALSE(result);
}

// Test rejection of negative Coinbase price
void test_spread_negative_coinbase_price() {
    double spread_abs, spread_pct;
    
    bool result = calc_spread(100.0, -102.0, &spread_abs, &spread_pct);
    
    TEST_ASSERT_FALSE(result);
}

// Test rejection of NaN Binance price
void test_spread_nan_binance_price() {
    double spread_abs, spread_pct;
    
    bool result = calc_spread(NAN, 100.0, &spread_abs, &spread_pct);
    
    TEST_ASSERT_FALSE(result);
}

// Test rejection of NaN Coinbase price
void test_spread_nan_coinbase_price() {
    double spread_abs, spread_pct;
    
    bool result = calc_spread(100.0, NAN, &spread_abs, &spread_pct);
    
    TEST_ASSERT_FALSE(result);
}

// Test rejection of infinite Binance price
void test_spread_inf_binance_price() {
    double spread_abs, spread_pct;
    
    bool result = calc_spread(INFINITY, 100.0, &spread_abs, &spread_pct);
    
    TEST_ASSERT_FALSE(result);
}

// Test rejection of infinite Coinbase price
void test_spread_inf_coinbase_price() {
    double spread_abs, spread_pct;
    
    bool result = calc_spread(100.0, INFINITY, &spread_abs, &spread_pct);
    
    TEST_ASSERT_FALSE(result);
}

// Test rejection of null spread_abs pointer
void test_spread_null_abs_pointer() {
    double spread_pct;
    
    bool result = calc_spread(100.0, 102.0, nullptr, &spread_pct);
    
    TEST_ASSERT_FALSE(result);
}

// Test rejection of null spread_pct pointer
void test_spread_null_pct_pointer() {
    double spread_abs;
    
    bool result = calc_spread(100.0, 102.0, &spread_abs, nullptr);
    
    TEST_ASSERT_FALSE(result);
}

// Test rejection of both null pointers
void test_spread_both_null_pointers() {
    bool result = calc_spread(100.0, 102.0, nullptr, nullptr);
    
    TEST_ASSERT_FALSE(result);
}

void setup() {
    UNITY_BEGIN();
    
    // Normal cases
    RUN_TEST(test_spread_normal_calculation);
    RUN_TEST(test_spread_negative);
    RUN_TEST(test_spread_zero);
    RUN_TEST(test_spread_large_values);
    
    // Zero price edge cases
    RUN_TEST(test_spread_zero_binance_price);
    RUN_TEST(test_spread_zero_coinbase_price);
    
    // Negative price edge cases
    RUN_TEST(test_spread_negative_binance_price);
    RUN_TEST(test_spread_negative_coinbase_price);
    
    // NaN edge cases
    RUN_TEST(test_spread_nan_binance_price);
    RUN_TEST(test_spread_nan_coinbase_price);
    
    // Infinity edge cases
    RUN_TEST(test_spread_inf_binance_price);
    RUN_TEST(test_spread_inf_coinbase_price);
    
    // Null pointer edge cases
    RUN_TEST(test_spread_null_abs_pointer);
    RUN_TEST(test_spread_null_pct_pointer);
    RUN_TEST(test_spread_both_null_pointers);
    
    UNITY_END();
}

void loop() {
    // Tests run once in setup
}
