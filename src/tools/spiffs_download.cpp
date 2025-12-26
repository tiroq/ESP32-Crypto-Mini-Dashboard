#include "spiffs_download.h"
#include "../config.h"

#if ENABLE_SCREENSHOT

#include "ui/ui_screenshot.h"
#include "app/app_scheduler.h"
#include <SPIFFS.h>
#include <Arduino.h>

void spiffs_check_download_command() {
    if (Serial.available()) {
        String cmd = Serial.readStringUntil('\n');
        cmd.trim();
        
        if (cmd == "SCREENSHOT") {
            Serial.println("[CMD] Taking screenshot...");
            
            // Pause scheduler to prevent interference during screenshot
            scheduler_pause();
            
            // Delete old screenshot to free space
            if (SPIFFS.exists("/dashboard.bmp")) {
                SPIFFS.remove("/dashboard.bmp");
                Serial.println("[CMD] Deleted old screenshot");
            }
            
            bool success = ui_take_screenshot("/dashboard.bmp");
            
            // Resume scheduler after screenshot
            scheduler_resume();
            
            if (success) {
                Serial.println("[CMD] Screenshot saved to /dashboard.bmp");
                Serial.println("[CMD] Use DOWNLOAD command to retrieve it");
            } else {
                Serial.println("[CMD] Screenshot failed!");
            }
        }
        else if (cmd == "DOWNLOAD") {
            // Pause network tasks to prevent interference
            scheduler_pause();
            delay(100);  // Let network tasks finish current operation
            
            Serial.println("[DOWNLOAD] Starting file transfer...");
            
            File file = SPIFFS.open("/dashboard.bmp", "r");
            if (!file) {
                Serial.println("ERROR: File not found");
                scheduler_resume();
                return;
            }
            
            size_t fileSize = file.size();
            Serial.print("SIZE:");
            Serial.println(fileSize);
            
            // Send file as hex chunks with markers
            uint8_t buffer[64];  // 64 bytes = 128 hex chars
            const char* hex = "0123456789ABCDEF";
            size_t totalSent = 0;
            size_t chunkCount = 0;
            
            Serial.println("DATA_START");
            while (file.available()) {
                int bytesRead = file.read(buffer, 64);
                
                if (bytesRead > 0) {
                    // Send chunk with marker
                    Serial.print("<SDATA>");
                    for (int i = 0; i < bytesRead; i++) {
                        Serial.print(hex[buffer[i] >> 4]);
                        Serial.print(hex[buffer[i] & 0x0F]);
                    }
                    Serial.println("</>");
                    
                    totalSent += bytesRead;
                    chunkCount++;
                    
                    // Only yield, no delays to maximize speed
                    yield();
                }
            }
            Serial.println("DATA_END");
            
            file.close();
            Serial.println("OK");
            
            // Resume network tasks
            scheduler_resume();
        }
        else if (cmd == "LIST") {
            File root = SPIFFS.open("/");
            File file = root.openNextFile();
            
            Serial.println("SPIFFS Files:");
            while (file) {
                Serial.print("  ");
                Serial.print(file.name());
                Serial.print(" - ");
                Serial.print(file.size());
                Serial.println(" bytes");
                file = root.openNextFile();
            }
            Serial.println("OK");
        }
    }
}

#endif // ENABLE_SCREENSHOT
