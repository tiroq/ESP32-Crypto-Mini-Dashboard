#ifndef NET_OTA_H
#define NET_OTA_H

#include "../config.h"

#if ENABLE_OTA

#include <Arduino.h>

/**
 * @brief OTA update status
 */
enum OTAStatus {
    OTA_IDLE,
    OTA_READY,
    OTA_UPDATING,
    OTA_SUCCESS,
    OTA_FAILED
};

/**
 * @brief Initialize OTA HTTP server
 * @return true if successful
 */
bool ota_init();

/**
 * @brief Handle OTA server requests (call in main loop)
 */
void ota_handle();

/**
 * @brief Get current OTA status
 */
OTAStatus ota_get_status();

/**
 * @brief Get OTA status message
 */
const char* ota_get_message();

/**
 * @brief Get update progress (0-100)
 */
int ota_get_progress();

#endif // ENABLE_OTA
#endif // NET_OTA_H
