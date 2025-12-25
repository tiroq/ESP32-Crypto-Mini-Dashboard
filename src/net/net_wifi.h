#ifndef NET_WIFI_H
#define NET_WIFI_H

#include <Arduino.h>

// Wi-Fi connection management
// TODO: Implement Wi-Fi connect/reconnect logic

bool net_wifi_ensure_connected();
int net_wifi_rssi();
bool net_wifi_is_connected();

#endif // NET_WIFI_H
