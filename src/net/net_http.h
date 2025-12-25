#ifndef NET_HTTP_H
#define NET_HTTP_H

#include <Arduino.h>

// HTTP client wrapper
// TODO: Implement HTTP GET wrapper with timeout support

bool http_get(const char* url, String& out, uint32_t timeout_ms);

#endif // NET_HTTP_H
