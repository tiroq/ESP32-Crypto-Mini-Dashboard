#include "net_http.h"
#include "../config.h"
#include <WiFi.h>
#include <WiFiClient.h>
#if ENABLE_HTTPS
#include <WiFiClientSecure.h>
#endif

// Helper: Parse URL into components
static bool parse_url(const char* url, bool& is_https, String& host, int& port, String& path) {
    String url_str(url);
    
    // Determine protocol
    if (url_str.startsWith("https://")) {
        is_https = true;
        url_str = url_str.substring(8);
    } else if (url_str.startsWith("http://")) {
        is_https = false;
        url_str = url_str.substring(7);
    } else {
        DEBUG_PRINTLN("[HTTP] ERROR: URL must start with http:// or https://");
        return false;
    }
    
    // Find path separator
    int path_idx = url_str.indexOf('/');
    String host_port;
    if (path_idx >= 0) {
        host_port = url_str.substring(0, path_idx);
        path = url_str.substring(path_idx);
    } else {
        host_port = url_str;
        path = "/";
    }
    
    // Parse host and port
    int port_idx = host_port.indexOf(':');
    if (port_idx >= 0) {
        host = host_port.substring(0, port_idx);
        port = host_port.substring(port_idx + 1).toInt();
    } else {
        host = host_port;
        port = is_https ? 443 : 80;
    }
    
    return true;
}

bool http_get(const char* url, String& out, uint32_t timeout_ms) {
    out = ""; // Clear output
    
    // Parse URL
    bool is_https;
    String host, path;
    int port;
    if (!parse_url(url, is_https, host, port, path)) {
        return false;
    }
    
    // DEBUG_PRINTF("[HTTP] GET %s://%s:%d%s\n", is_https ? "https" : "http", 
    //               host.c_str(), port, path.c_str());
    
    unsigned long start_ms = millis();
    
    // Create appropriate client
    WiFiClient* client = nullptr;
#if ENABLE_HTTPS
    WiFiClientSecure* secure_client = nullptr;
    
    if (is_https) {
        secure_client = new WiFiClientSecure();
        // PROTOTYPE: Skip certificate validation for simplicity
        // TODO: For production, use setCACert() with proper certificates
        secure_client->setInsecure();
        client = secure_client;
        // DEBUG_PRINTLN("[HTTP] Using HTTPS (insecure mode)");
    } else {
        client = new WiFiClient();
    }
#else
    // HTTPS disabled - always use plain HTTP
    if (is_https) {
        DEBUG_PRINTLN("[HTTP] ERROR: HTTPS disabled, use HTTP URLs");
        return false;
    }
    client = new WiFiClient();
#endif
    
    // Set connection timeout
    client->setTimeout(timeout_ms / 1000); // WiFiClient uses seconds
    
    // Connect to server
    // DEBUG_PRINTF("[HTTP] Connecting to %s:%d...\n", host.c_str(), port);
    if (!client->connect(host.c_str(), port)) {
        DEBUG_PRINTF("[HTTP] Connection failed (elapsed: %lu ms)\n", millis() - start_ms);
        delete client;
        return false;
    }
    
    unsigned long connect_ms = millis() - start_ms;
    // DEBUG_PRINTF("[HTTP] Connected (took %lu ms)\n", connect_ms);
    
    // Send HTTP request
    client->printf("GET %s HTTP/1.1\r\n", path.c_str());
    client->printf("Host: %s\r\n", host.c_str());
    client->println("Connection: close");
    client->println("User-Agent: ESP32-CryptoDash/1.0");
    client->println();
    
    // Wait for response with timeout
    unsigned long wait_start = millis();
    while (!client->available() && (millis() - wait_start) < timeout_ms) {
        delay(1);
        if (!client->connected()) {
            DEBUG_PRINTLN("[HTTP] Server disconnected while waiting for response");
            delete client;
            return false;
        }
    }
    
    if (!client->available()) {
        DEBUG_PRINTLN("[HTTP] Timeout waiting for response");
        delete client;
        return false;
    }
    
    // Read status line
    String status_line = client->readStringUntil('\n');
    status_line.trim();
    // DEBUG_PRINTF("[HTTP] Status: %s\n", status_line.c_str());
    
    // Parse status code (handle both "HTTP/1.1 200 OK" and "HTTP/1.1 200")
    int first_space = status_line.indexOf(' ');
    if (first_space < 0) {
        DEBUG_PRINTLN("[HTTP] Invalid status line format");
        delete client;
        return false;
    }
    
    int second_space = status_line.indexOf(' ', first_space + 1);
    String status_code_str;
    
    if (second_space < 0) {
        // No second space - format is "HTTP/1.1 200" without reason phrase
        status_code_str = status_line.substring(first_space + 1);
    } else {
        // Format is "HTTP/1.1 200 OK" with reason phrase
        status_code_str = status_line.substring(first_space + 1, second_space);
    }
    
    status_code_str.trim();
    int status_code = status_code_str.toInt();
    
    if (status_code != 200) {
        DEBUG_PRINTF("[HTTP] Non-200 status: %d\n", status_code);
        delete client;
        return false;
    }
    
    // Skip headers
    while (client->available()) {
        String header = client->readStringUntil('\n');
        header.trim();
        if (header.length() == 0) {
            break; // Empty line = end of headers
        }
    }
    
    // Read response body
    int bytes_read = 0;
    while (client->available() || client->connected()) {
        if (client->available()) {
            char c = client->read();
            out += c;
            bytes_read++;
        } else {
            delay(1);
        }
        
        // Timeout check
        if (millis() - start_ms > timeout_ms) {
            DEBUG_PRINTLN("[HTTP] Timeout reading body");
            delete client;
            return false;
        }
    }
    
    unsigned long elapsed_ms = millis() - start_ms;
    // DEBUG_PRINTF("[HTTP] Success: %d bytes in %lu ms\n", bytes_read, elapsed_ms);
    
    client->stop();
    delete client;
    
    return true;
}
