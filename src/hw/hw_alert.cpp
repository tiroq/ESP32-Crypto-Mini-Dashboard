#include "hw_alert.h"
#include "../config.h"

// ESP32-2432S028 does not have a buzzer wired
// This is a safe no-op implementation (Task 9.1)

void hw_alert_init() {
    DEBUG_PRINTLN("[ALERT] Alert hardware initialized (no buzzer present)");
}

void hw_alert_beep(uint16_t ms_on, uint16_t ms_off, uint8_t count) {
    // Safe no-op - no buzzer hardware on ESP32-2432S028
    // Log alert trigger for debugging
    DEBUG_PRINTF("[ALERT] Beep triggered: %dms ON, %dms OFF, %d times\n", 
                  ms_on, ms_off, count);
}
