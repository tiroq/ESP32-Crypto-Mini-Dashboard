#ifndef NET_HTTP_H
#define NET_HTTP_H

#include <Arduino.h>

// HTTP client wrapper (Task 5.2)
// Supports both HTTP and HTTPS with configurable timeouts
// Uses WiFiClientSecure for HTTPS (setInsecure for prototype)

// Perform HTTP GET request
// url: Full URL (http:// or https://)
// out: Response body (cleared before use)
// timeout_ms: Total timeout for connection + read (default 10s)
// Returns: true on success (200 OK), false on any error
bool http_get(const char* url, String& out, uint32_t timeout_ms = 10000);

#endif // NET_HTTP_H
