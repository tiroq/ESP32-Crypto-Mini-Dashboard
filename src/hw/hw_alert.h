#ifndef HW_ALERT_H
#define HW_ALERT_H

#include <Arduino.h>

// Alert hardware abstraction (buzzer, LED, etc.)
// TODO: Implement alert mechanisms

void hw_alert_init();
void hw_alert_beep(uint16_t ms_on, uint16_t ms_off, uint8_t count);

#endif // HW_ALERT_H
