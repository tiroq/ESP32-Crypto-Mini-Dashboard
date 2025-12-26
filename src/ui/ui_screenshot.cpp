#include "ui_screenshot.h"
#include "../config.h"

#if ENABLE_SCREENSHOT

#include <lvgl.h>
#include <src/extra/lv_extra.h>
#include <SPIFFS.h>
#include <FS.h>
#include "../hw/hw_display.h"

// SPIFFS initialization flag
static bool spiffs_initialized = false;

bool ui_screenshot_init() {
    if (spiffs_initialized) {
        Serial.println("[SCREENSHOT] SPIFFS already initialized");
        return true;
    }
    
    Serial.println("[SCREENSHOT] Initializing SPIFFS...");
    
    if (!SPIFFS.begin(true)) {  // true = format on mount failure
        Serial.println("[SCREENSHOT] SPIFFS mount failed!");
        return false;
    }
    
    size_t total = SPIFFS.totalBytes();
    size_t used = SPIFFS.usedBytes();
    Serial.printf("[SCREENSHOT] SPIFFS mounted: %zu KB total, %zu KB used\n", 
                  total / 1024, used / 1024);
    
    spiffs_initialized = true;
    return true;
}

// BMP file header structures (packed, little-endian)
#pragma pack(push, 1)
struct BMPFileHeader {
    uint16_t signature;   // 'BM'
    uint32_t file_size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t data_offset;
};

struct BMPInfoHeader {
    uint32_t header_size;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bits_per_pixel;
    uint32_t compression;
    uint32_t image_size;
    int32_t x_pixels_per_m;
    int32_t y_pixels_per_m;
    uint32_t colors_used;
    uint32_t colors_important;
};
#pragma pack(pop)

bool ui_take_screenshot(const char* path) {
    if (!spiffs_initialized) {
        Serial.println("[SCREENSHOT] ERROR: SPIFFS not initialized");
        return false;
    }
    
    if (!path || path[0] == '\0') {
        Serial.println("[SCREENSHOT] ERROR: Invalid path");
        return false;
    }
    
    Serial.printf("[SCREENSHOT] Taking screenshot: %s\n", path);
    Serial.printf("[SCREENSHOT] Free heap: %d bytes\n", ESP.getFreeHeap());
    
    // Get display dimensions
    uint32_t width = 320;
    uint32_t height = 240;
    
    Serial.printf("[SCREENSHOT] Display: %ux%u pixels\n", width, height);
    
    // Try to get screenshot from LVGL layer
    lv_obj_t* scr = lv_scr_act();
    if (!scr) {
        Serial.println("[SCREENSHOT] ERROR: No active screen");
        return false;
    }
    
    Serial.println("[SCREENSHOT] Capturing from LVGL active screen...");
    
    // Calculate BMP parameters (24-bit RGB)
    uint32_t row_size = ((width * 3 + 3) / 4) * 4;  // 4-byte aligned
    uint32_t pixel_data_size = row_size * height;
    uint32_t file_size = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + pixel_data_size;
    
    Serial.printf("[SCREENSHOT] BMP file size: %u bytes\n", file_size);
    
    // Check SPIFFS space
    size_t total = SPIFFS.totalBytes();
    size_t used = SPIFFS.usedBytes();
    size_t available = total - used;
    Serial.printf("[SCREENSHOT] SPIFFS: %zu total, %zu used, %zu available\n", total, used, available);
    
    if (available < file_size) {
        Serial.printf("[SCREENSHOT] ERROR: Not enough space! Need %u bytes, only %zu available\n", 
                     file_size, available);
        return false;
    }
    
    // Open file for writing
    File file = SPIFFS.open(path, FILE_WRITE);
    if (!file) {
        Serial.printf("[SCREENSHOT] ERROR: Failed to open file: %s\n", path);
        return false;
    }
    Serial.println("[SCREENSHOT] File opened successfully");
    
    // Write BMP file header
    BMPFileHeader file_header = {
        .signature = 0x4D42,  // 'BM'
        .file_size = file_size,
        .reserved1 = 0,
        .reserved2 = 0,
        .data_offset = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader)
    };
    file.write((uint8_t*)&file_header, sizeof(file_header));
    
    // Write BMP info header
    BMPInfoHeader info_header = {
        .header_size = sizeof(BMPInfoHeader),
        .width = (int32_t)width,
        .height = (int32_t)height,
        .planes = 1,
        .bits_per_pixel = 24,
        .compression = 0,
        .image_size = pixel_data_size,
        .x_pixels_per_m = 2835,
        .y_pixels_per_m = 2835,
        .colors_used = 0,
        .colors_important = 0
    };
    file.write((uint8_t*)&info_header, sizeof(info_header));
    
    Serial.println("[SCREENSHOT] Headers written, capturing screen in 6 passes...");
    
    // Allocate row buffer for RGB888 BMP data
    uint8_t* row_buffer = (uint8_t*)malloc(row_size);
    if (!row_buffer) {
        Serial.println("[SCREENSHOT] ERROR: Failed to allocate row buffer");
        file.close();
        return false;
    }
    
    // Allocate buffer for one 40-row chunk
    const uint32_t chunk_rows = 40;
    lv_color_t* chunk_buffer = (lv_color_t*)malloc(width * chunk_rows * sizeof(lv_color_t));
    
    if (!chunk_buffer) {
        Serial.println("[SCREENSHOT] ERROR: Failed to allocate chunk buffer");
        free(row_buffer);
        file.close();
        return false;
    }
    
    // Global state for capture callback
    static lv_color_t* g_chunk_buffer = NULL;
    static uint32_t g_capture_y_start = 0;
    static uint32_t g_capture_y_end = 0;
    
    g_chunk_buffer = chunk_buffer;
    
    // Set up capture callback
    hw_display_start_capture([](int32_t x, int32_t y, int32_t w, int32_t h, const lv_color_t* pixels) {
        for (int32_t row = 0; row < h; row++) {
            int32_t screen_y = y + row;
            if (screen_y >= (int32_t)g_capture_y_start && screen_y < (int32_t)g_capture_y_end) {
                int32_t buffer_y = screen_y - g_capture_y_start;
                for (int32_t col = 0; col < w; col++) {
                    int32_t screen_x = x + col;
                    if (screen_x < 320 && buffer_y < 40) {
                        g_chunk_buffer[buffer_y * 320 + screen_x] = pixels[row * w + col];
                    }
                }
            }
        }
    });
    
    // Capture and write each chunk in BMP bottom-up order
    // Chunks: 0=rows 0-39, 1=40-79, 2=80-119, 3=120-159, 4=160-199, 5=200-239
    // BMP needs: chunk 5, then 4, then 3, then 2, then 1, then 0
    const uint32_t num_chunks = 6;
    
    for (int32_t chunk = num_chunks - 1; chunk >= 0; chunk--) {
        g_capture_y_start = chunk * chunk_rows;
        g_capture_y_end = g_capture_y_start + chunk_rows;
        
        memset(chunk_buffer, 0, width * chunk_rows * sizeof(lv_color_t));
        
        // Invalidate this region
        lv_area_t area;
        area.x1 = 0;
        area.y1 = g_capture_y_start;
        area.x2 = width - 1;
        area.y2 = g_capture_y_end - 1;
        lv_obj_invalidate_area(lv_scr_act(), &area);
        
        // Force refresh
        for (int i = 0; i < 10; i++) {
            lv_refr_now(NULL);
            yield();
        }
        
        // Write this chunk's rows in bottom-up order (within the chunk)
        for (int32_t local_y = chunk_rows - 1; local_y >= 0; local_y--) {
            for (uint32_t x = 0; x < width; x++) {
                lv_color_t pixel = chunk_buffer[local_y * width + x];
                uint16_t rgb565 = pixel.full;
                
                uint8_t r = ((rgb565 >> 11) & 0x1F);
                uint8_t g = ((rgb565 >> 5) & 0x3F);
                uint8_t b = (rgb565 & 0x1F);
                
                r = (r << 3) | (r >> 2);
                g = (g << 2) | (g >> 4);
                b = (b << 3) | (b >> 2);
                
                row_buffer[x * 3 + 0] = b;
                row_buffer[x * 3 + 1] = g;
                row_buffer[x * 3 + 2] = r;
            }
            
            for (uint32_t i = width * 3; i < row_size; i++) {
                row_buffer[i] = 0;
            }
            
            file.write(row_buffer, row_size);
        }
        
        Serial.printf("[SCREENSHOT] Progress: %d%%\r", (int)(((num_chunks - chunk) * 100) / num_chunks));
    }
    
    hw_display_stop_capture();
    free(chunk_buffer);
    
    Serial.println("\n[SCREENSHOT] Display captured successfully");
    
    free(row_buffer);
    file.close();
    
    Serial.printf("[SCREENSHOT] Saved: %s (%u bytes)\n", path, file_size);
    return true;
}

#endif // ENABLE_SCREENSHOT
