#ifndef NET_WIFI_H
#define NET_WIFI_H

#include <Arduino.h>

// Wi-Fi connection management (Task 5.1)
// Provides connection retry logic with rate-limited logging

// Ensure Wi-Fi is connected, attempt reconnect if needed
// Returns true if connected, false if connection failed
bool net_wifi_ensure_connected();

// Get current RSSI (signal strength) in dBm
// Returns -100 if not connected
int net_wifi_rssi();

// Check if Wi-Fi is currently connected
bool net_wifi_is_connected();

// Initialize Wi-Fi (call once at startup)
void net_wifi_init();

#endif // NET_WIFI_H
