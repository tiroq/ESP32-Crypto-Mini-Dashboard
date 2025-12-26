# Screenshot Feature

## Overview

The ESP32 Crypto Mini Dashboard supports software screenshots via serial commands. Screenshots are captured from the LVGL display, saved to SPIFFS as BMP files, and downloaded to your computer using the included Python script.

## Quick Start

### Requirements

- Python 3.x
- pyserial library: `pip3 install pyserial`
- Serial connection to ESP32 at 115200 baud

### Take and Download Screenshot

```bash
python3 download_screenshot.py -s
```

This will:
1. Connect to the ESP32 via serial
2. Send `SCREENSHOT` command to capture the display
3. Wait for capture to complete (~2 seconds)
4. Download the file as `dashboard.bmp` (~15-20 seconds)
5. Display progress during transfer

### Download Only

If a screenshot already exists on the device:

```bash
python3 download_screenshot.py
```

## Serial Commands

Connect to the ESP32 serial monitor at 115200 baud and use these commands:

### SCREENSHOT

Takes a screenshot of the current display and saves it to `/dashboard.bmp` on SPIFFS.

```
SCREENSHOT
```

**Output:**
```
[CMD] Taking screenshot...
[SCHEDULER] Network task paused
[CMD] Deleted old screenshot
[SCREENSHOT] Taking screenshot: /dashboard.bmp
[SCREENSHOT] Free heap: 157844 bytes
[SCREENSHOT] Display: 320x240 pixels
[SCREENSHOT] BMP file size: 230454 bytes
[SCREENSHOT] SPIFFS: 1318001 total, 0 used, 1318001 available
[SCREENSHOT] File opened successfully
[SCREENSHOT] Headers written, capturing screen in 6 passes...
[SCREENSHOT] Progress: 100%
[SCREENSHOT] Display captured successfully
[SCREENSHOT] Saved: /dashboard.bmp (230454 bytes)
[SCHEDULER] Network task resumed
[CMD] Screenshot saved to /dashboard.bmp
```

### DOWNLOAD

Downloads the screenshot file via hex-encoded serial transfer.

```
DOWNLOAD
```

**Output:**
```
[SCHEDULER] Network task paused
[DOWNLOAD] Starting file transfer...
SIZE:230454
DATA_START
<SDATA>hex_data</>
...
DATA_END
OK
[SCHEDULER] Network task resumed
```

### LIST

Lists all files on SPIFFS with their sizes.

```
LIST
```

**Output:**
```
SPIFFS Files:
  dashboard.bmp - 230454 bytes
OK
```

## How It Works

### Screenshot Capture

1. **Pause Network**: Scheduler suspends network tasks to prevent interference during capture
2. **Cleanup**: Automatically deletes old screenshot to free SPIFFS space
3. **Memory Check**: Verifies sufficient heap and SPIFFS space (needs ~230KB)
4. **Chunked Capture**: Captures screen in 6 passes of 40 rows each (240 total rows)
5. **LVGL Interception**: Hooks into LVGL's display flush callback to capture rendered pixels
6. **BMP Encoding**: Converts RGB565 pixels to 24-bit RGB888 BMP format with proper headers
7. **Direct Write**: Writes each chunk directly to SPIFFS in correct bottom-up BMP order
8. **Resume Network**: Restores normal operation after completion

### File Transfer Protocol

- **Encoding**: Binary data → hexadecimal text (prevents serial corruption)
- **Chunk Size**: 64 bytes per chunk = 128 hex characters per line
- **Format**: `<SDATA>HEXDATA</>` markers for each chunk
- **Progress**: SIZE announcement, DATA_START/DATA_END markers
- **Validation**: Python script validates hex characters and filters corrupted chunks
- **Speed**: ~230KB transfers in 15-20 seconds at 115200 baud
- **Reliability**: 30-second timeout watchdog, automatic retry capability

## Files

### Core Implementation
- `src/ui/ui_screenshot.h` - Screenshot API declarations
- `src/ui/ui_screenshot.cpp` - Chunked capture and BMP encoding
- `src/hw/hw_display.h/cpp` - Display flush callback interception
- `src/tools/spiffs_download.cpp` - Serial command handler (SCREENSHOT/DOWNLOAD/LIST)
- `src/app/app_scheduler.cpp` - Network task pause/resume functions

### Python Client
- `download_screenshot.py` - Serial download script with hex decoding and progress display

## Technical Details

### BMP File Format

- **Resolution**: 320×240 pixels
- **Color Depth**: 24-bit RGB (RGB888)
- **File Size**: 230,454 bytes
  - 54 bytes: BMP headers (14-byte file header + 40-byte info header)
  - 230,400 bytes: Pixel data (960 bytes per row × 240 rows)
- **Pixel Format**: BGR format (bottom-up scanlines)
- **Row Alignment**: 4-byte aligned (no padding needed for 320×3=960 bytes)
- **Color Conversion**: RGB565 → RGB888 with proper bit expansion

### Memory Architecture

**Heap Usage:**
- Row buffer: ~960 bytes (320 pixels × 3 bytes RGB888)
- Chunk buffer: ~25.6 KB (320 pixels × 40 rows × 2 bytes RGB565)
- Total: ~26.5 KB temporary allocation (freed after capture)
- Free heap during capture: ~158 KB (sufficient headroom)

**SPIFFS Storage:**
- Partition size: ~1.3 MB (1,318,001 bytes)
- Screenshot size: ~230 KB
- Auto-cleanup: Old screenshot deleted before new capture
- Available space: ~1 MB for future features

**Capture Method:**
- Cannot allocate full screen buffer (320×240×2 = 153 KB exceeds available heap)
- Uses chunked approach: 6 passes of 40 rows each
- Matches LVGL's internal buffer size (12,800 pixels = 40 rows × 320 pixels)
- Each chunk captured via display flush callback interception

### Capture Process

1. **Hook Installation**: Register callback in `hw_display.cpp` flush function
2. **Region Invalidation**: Mark specific 40-row region as dirty
3. **Forced Refresh**: Call `lv_refr_now()` to trigger LVGL redraw
4. **Pixel Capture**: Callback intercepts rendered pixels during flush to display
5. **Chunk Storage**: Pixels copied to temporary 40-row buffer
6. **BMP Writing**: Convert RGB565→RGB888 and write rows in bottom-up order
7. **Iteration**: Repeat for all 6 chunks (rows 200-239, 160-199, ..., 0-39)

### Build Configuration

Required in `platformio.ini`:
```inERROR: Failed to allocate chunk buffer"
- **Cause**: Insufficient heap memory for 40-row chunk buffer (~26 KB)
- **Solution**: This should not occur with ~158 KB free heap. Check for memory leaks with `ESP.getFreeHeap()`

### "ERROR: Not enough space!"
- **Cause**: SPIFFS full (needs ~230 KB)
- **Solution**: Old screenshot is auto-deleted, but verify with `LIST` command. Use `SPIFFS.format()` if needed

### Black or Incomplete Image
- **Cause**: Network task interference during capture
- **Solution**: Scheduler pause/resume is automatic. Verify "[SCHEDULER] Network task paused/resumed" messages appear

### Transfer Corruption
- **Cause**: Non-hex characters in serial stream (log messages mixed with data)
- **Solution**: Python script filters automatically. Ensure scheduler is paused during DOWNLOAD

### Repeated Pattern (Header 6 Times)
- **Cause**: All 6 chunks capturing the same region (rows 0-39)
- **Solution**: Fixed in current implementation - each chunk captures different Y range

### "Can't open image" / File Corruption
- **Cause**: BMP file structure invalid (wrong headers or row order)
- **Solution**: Ensure chunks written in correct bottom-up order (chunk 5→4→3→2→1→0)

### Slow Download Speed
- **Cause**: Hex encoding doubles data size, serial speed limited to 115200 baud
- **Solution**: This is expected (~15-20 seconds for 230 KB). Future: use binary transfer or HTTP

### Python Script Timeout
- **Cause**: No response from ESP32 within 30 seconds
- **Solution**: Check serial connection, verify ESP32 is running. Try manual `SCREENSHOT` command first

## Example Workflow

```bash
# Terminal 1 - Monitor ESP32 (optional)
pio device monitor --baud 115200

# Terminal 2 - Take screenshot and download
python3 download_screenshot.py -s
```

**Expected Output:**
```
Connecting to /dev/cu.usbserial-10 at 115200...
Taking screenshot...
Screenshot taken successfully!
Sending DOWNLOAD command...
File size: 230454 bytes
Receiving data chunks...
Progress: 100% (230454/230454 bytes, 3601 chunks)
✓ Saved dashboard.bmp (230454 bytes)
```

**View the screenshot:**
```bash
open dashboard.bmp  # macOS
xdg-open dashboard.bmp  # Linux
start dashboard.bmp  # Windows
```

## Advanced Usage

### Manual Serial Commands

```bash
# Connect to serial monitor
pio device monitor --baud 115200

# Take screenshot
SCREENSHOT

# List files
LIST

# Download (then run Python script to decode)
DOWNLOAD
```

### Integration Example

Add to your code for programmatic screenshots:

```cpp
#include "ui/ui_screenshot.h"

// Triggered by button press or event
void onScreenshotButton() {
    if (ui_take_screenshot("/dashboard.bmp")) {
        Serial.println("[APP] Screenshot ready for download");
    } else {
        Serial.println("[APP] Screenshot failed");
    }
}
```

### Python Script Options

```bash
# Screenshot + download
python3 download_screenshot.py -s

# Download only
python3 download_screenshot.py

# Specify custom serial port
python3 download_screenshot.py -s -p /dev/ttyUSB0

# Help
python3 download_screenshot.py --help
``apshot (requires 153 KB heap)
- **Serial Speed**: Transfer limited to ~12 KB/s effective (hex encoding doubles data size)
- **Single File**: Only one screenshot stored at a time (auto-deleted on next capture)
- **No Compression**: BMP format used (uncompressed, large file size)

## Future Enhancements

- [ ] JPEG compression to reduce file size (~10-20 KB vs 230 KB)
- [ ] Faster transfer via binary protocol or HTTP download
- [ ] Timestamp-based filenames for multiple screenshots
- [ ] Lower resolution option (160×120) for testing
- [ ] Direct file write during capture (eliminate chunk buffer)
- [ ] HTTP server endpoint for web-based download
- [ ] Automatic periodic screenshots with rotation

## Troubleshooting

### "SPIFFS mount failed"
- SPIFFS partition may not exist - check partition table
- Filesystem may be corrupted - use `SPIFFS.format()` to reinitialize

### "lv_snapshot_take failed"
- Insufficient heap memory - check with `ESP.getFreeHeap()`  
- Invalid screen object - ensure UI is fully initialized

### "File not found after write"
- SPIFFS full - check with `SPIFFS.totalBytes()` and `SPIFFS.usedBytes()`
- Write error - verify SPIFFS is mounted and writable

### Build Errors "lv_snapshot_take not declared"
- `LV_USE_SNAPSHOT` not enabled - check `lv_conf.h` and build flags
- LVGL library cache issue - delete `.pio/libdeps` and rebuild
- Missing include path - verify `-I./` in `build_flags`
