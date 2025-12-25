#ifndef APP_ALERTS_H
#define APP_ALERTS_H

#include <Arduino.h>

// Alert engine with cooldown and threshold checking (Task 9.1)

/**
 * @brief Initialize alert engine
 * Should be called during app initialization
 */
void alerts_init();

/**
 * @brief Alert monitoring task (runs in FreeRTOS task)
 * Checks model snapshot every 300ms for threshold violations
 * Implements per-symbol cooldown to prevent spam
 * @param pvParameters Task parameters (unused)
 */
void alert_task(void* pvParameters);

/**
 * @brief Check if any alert is currently active
 * Used by UI to show visual indicator
 * @return true if alert active, false otherwise
 */
bool alerts_is_active();

/**
 * @brief Get the number of active alerts
 * @return Count of symbols currently in alert state
 */
int alerts_get_active_count();

#endif // APP_ALERTS_H
