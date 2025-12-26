#include "net_ota.h"
#include "net_dashboard.h"
#include "../config.h"

#if ENABLE_OTA

#include <WiFi.h>
#include <WebServer.h>
#include <Update.h>

static WebServer server(8080);
static OTAStatus current_status = OTA_IDLE;
static String status_message = "";
static int update_progress = 0;

// Minimal HTML page for firmware upload (stored in PROGMEM to save DRAM)
const char upload_html[] PROGMEM = R"(
<!DOCTYPE html>
<html><head><meta name="viewport" content="width=device-width,initial-scale=1">
<style>body{font-family:Arial;text-align:center;background:#181A20;color:#F0B90B;padding:20px}
h1{color:#F0B90B}input{margin:20px 0;padding:10px}
button{background:#F0B90B;color:#000;border:none;padding:15px 30px;font-size:16px;cursor:pointer}
button:hover{background:#FFD700}
.nav{margin:20px 0}
.nav a{background:#2B3139;color:#F0F0F0;padding:10px 20px;text-decoration:none;border-radius:4px;margin:0 5px}
.nav a:hover{background:#3B4149}</style></head>
<body><h1>OTA Update</h1><p>ESP32 Crypto Dashboard</p>
<div class="nav">
<a href="/dashboard">Dashboard</a>
<a href="/">OTA Update</a>
</div>
<form method="POST" action="/update" enctype="multipart/form-data">
<input type="file" name="firmware" accept=".bin" required><br>
<button type="submit">Upload Firmware</button></form></body></html>
)";

bool ota_init() {
    if (WiFi.status() != WL_CONNECTED) {
        current_status = OTA_IDLE;
        status_message = "WiFi not connected";
        return false;
    }

    // Serve upload page
    server.on("/", HTTP_GET, []() {
        server.send(200, "text/html", upload_html);
    });

    // Initialize dashboard routes
    dashboard_init(&server);

    // Handle firmware upload
    server.on("/update", HTTP_POST, 
        []() {
            server.sendHeader("Connection", "close");
            if (Update.hasError()) {
                server.send(500, "text/plain", status_message);
            } else {
                server.send(200, "text/plain", "OK");
                delay(500);
                ESP.restart();
            }
        },
        []() {
            HTTPUpload& upload = server.upload();
            
            if (upload.status == UPLOAD_FILE_START) {
                DEBUG_PRINTF("[OTA] Starting update: %s\n", upload.filename.c_str());
                current_status = OTA_UPDATING;
                status_message = "Receiving firmware...";
                update_progress = 0;
                
                if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
                    Update.printError(Serial);
                    status_message = "Update begin failed";
                    current_status = OTA_FAILED;
                }
            } 
            else if (upload.status == UPLOAD_FILE_WRITE) {
                if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
                    Update.printError(Serial);
                    status_message = "Write failed";
                    current_status = OTA_FAILED;
                } else {
                    update_progress = (Update.progress() * 100) / Update.size();
                }
            } 
            else if (upload.status == UPLOAD_FILE_END) {
                if (Update.end(true)) {
                    DEBUG_PRINTF("[OTA] Update Success: %u bytes\n", upload.totalSize);
                    current_status = OTA_SUCCESS;
                    status_message = "Update complete!";
                    update_progress = 100;
                } else {
                    Update.printError(Serial);
                    status_message = "Update end failed";
                    current_status = OTA_FAILED;
                }
            }
        }
    );

    server.begin();
    current_status = OTA_READY;
    status_message = "Server started on port 8080";
    DEBUG_PRINTF("[OTA] HTTP server started on http://%s:8080\n", WiFi.localIP().toString().c_str());
    
    return true;
}

void ota_handle() {
    if (current_status != OTA_IDLE) {
        server.handleClient();
    }
}

OTAStatus ota_get_status() {
    return current_status;
}

const char* ota_get_message() {
    return status_message.c_str();
}

int ota_get_progress() {
    return update_progress;
}

#endif // ENABLE_OTA
