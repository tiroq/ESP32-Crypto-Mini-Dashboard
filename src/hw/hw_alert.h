#ifndef HW_ALERT_H
#define HW_ALERT_H

#include <Arduino.h>

// Hardware alert module - audio/visual alerts (Task 9.1)
// Safe no-op implementation since no buzzer is wired on ESP32-2432S028

/**
 * @brief Initialize alert hardware (safe no-op if not wired)
 */
void hw_alert_init();

/**
 * @brief Trigger a beep pattern (safe no-op if not wired)
 * @param ms_on Duration of beep in milliseconds
 * @param ms_off Duration of silence between beeps in milliseconds
 * @param count Number of beeps
 */
void hw_alert_beep(uint16_t ms_on, uint16_t ms_off, uint8_t count);

#endif // HW_ALERT_H
