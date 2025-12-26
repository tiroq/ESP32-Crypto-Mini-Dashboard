#ifndef NET_DASHBOARD_H
#define NET_DASHBOARD_H

#include "../config.h"

#if ENABLE_OTA

#include <WebServer.h>

// Initialize web dashboard routes on the given server
void dashboard_init(WebServer* server);

// Handle WebSocket connections (if implemented)
void dashboard_handle();

#endif // ENABLE_OTA

#endif // NET_DASHBOARD_H
