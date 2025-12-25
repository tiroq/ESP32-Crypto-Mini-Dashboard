#!/bin/bash

echo "================================================"
echo "ESP32 Firmware Upload Helper"
echo "================================================"
echo ""
echo "To enter bootloader mode:"
echo "  1. Hold BOOT button on the ESP32"
echo "  2. While holding BOOT, press and release RESET"
echo "  3. Release BOOT button"
echo ""
echo "Press ENTER when ready to upload..."
read

echo "Uploading firmware..."
pio run -t upload --upload-port /dev/cu.usbserial-11340
