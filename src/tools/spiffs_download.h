#ifndef SPIFFS_DOWNLOAD_H
#define SPIFFS_DOWNLOAD_H

// Serial commands:
// - SCREENSHOT: Take a screenshot and save to /dashboard.bmp
// - DOWNLOAD: Download the screenshot file
// - LIST: List all files on SPIFFS
void spiffs_check_download_command();

#endif
