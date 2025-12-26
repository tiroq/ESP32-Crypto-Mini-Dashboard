#ifndef APP_SCHEDULER_H
#define APP_SCHEDULER_H

#include <Arduino.h>

/**
 * @file app_scheduler.h
 * @brief Task scheduler for managing network and update tasks (Task 8.1)
 * 
 * Creates FreeRTOS tasks for:
 * - Periodic network data fetching (prices, funding rates)
 * - Keeping UI/LVGL loop separate from network operations
 */

/**
 * @brief Initialize and start scheduled tasks
 * 
 * Creates FreeRTOS tasks for:
 * - net_task: Periodic fetch of spot prices and funding rates
 * 
 * Tasks run independently from the main LVGL/UI loop to prevent blocking.
 * All network operations happen in net_task with proper delays and backoff.
 */
void scheduler_init();

/**
 * @brief Stop all scheduled tasks (cleanup)
 * 
 * Stops and deletes FreeRTOS tasks created by scheduler_init().
 * Should be called before system reset or deep sleep.
 */
void scheduler_stop();

/**
 * @brief Temporarily pause network tasks
 * 
 * Suspends the network task to prevent interference with serial downloads.
 */
void scheduler_pause();

/**
 * @brief Resume network tasks
 * 
 * Resumes the network task after pause.
 */
void scheduler_resume();

#endif // APP_SCHEDULER_H
