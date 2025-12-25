#ifndef NET_TIME_H
#define NET_TIME_H

#include <Arduino.h>

// NTP time synchronization
// TODO: Implement NTP time sync

void time_init();
void time_sync();
String time_get_string();

#endif // NET_TIME_H
