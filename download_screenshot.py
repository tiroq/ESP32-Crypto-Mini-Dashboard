#!/usr/bin/env python3
"""
Download screenshot from ESP32 via serial.

Usage:
    python3 download_screenshot.py [options] [port] [baudrate]
    
Options:
    -s, --screenshot    Take a screenshot before downloading
    
Examples:
    python3 download_screenshot.py -s
    python3 download_screenshot.py --screenshot /dev/cu.usbserial-10 115200
    python3 download_screenshot.py /dev/cu.usbserial-10 115200
"""

import serial
import sys
import time
import argparse

def download_screenshot(port='/dev/cu.usbserial-10', baudrate=115200, take_screenshot=False):
    print(f"Connecting to {port} at {baudrate}...")
    
    try:
        ser = serial.Serial(port, baudrate, timeout=5)  # Increased timeout to 5s
        time.sleep(2)  # Wait for connection
        
        # Take screenshot first if requested
        if take_screenshot:
            print("Taking screenshot...")
            ser.write(b'SCREENSHOT\n')
            
            # Wait for screenshot completion
            while True:
                line = ser.readline().decode('utf-8', errors='ignore').strip()
                if not line:
                    continue
                print(f"< {line}")
                
                if "[CMD] Screenshot saved" in line or "[CMD] Screenshot failed" in line:
                    break
                    
            if "[CMD] Screenshot failed" in line:
                print("Screenshot failed!")
                ser.close()
                return False
                
            print("Screenshot taken successfully!\n")
            time.sleep(0.5)
        
        print("Sending DOWNLOAD command...")
        ser.write(b'DOWNLOAD\n')
        
        # Read response
        file_data = bytearray()
        in_data = False
        size = 1  # Default size, will be updated from SIZE: response
        
        while True:
            line = ser.readline().decode('utf-8', errors='ignore').strip()
            
            if not line:
                continue
                
            print(f"< {line}")
            
            if line.startswith("SIZE:"):
                size = int(line.split(':')[1])
                print(f"File size: {size} bytes")
                
            elif line == "DATA_START":
                print("Receiving data chunks...")
                in_data = True
                last_update = time.time()
                last_data_time = time.time()
                chunk_count = 0
                empty_count = 0
                
                # Read chunks until DATA_END
                while True:
                    line = ser.readline().decode('utf-8', errors='ignore').strip()
                    
                    if line == "DATA_END":
                        print("\nReceived DATA_END marker")
                        break
                    
                    # Check for timeout (30 seconds with no data)
                    if time.time() - last_data_time > 30:
                        print("\nERROR: Transfer timeout (no data for 30s)")
                        break
                    
                    # Track empty lines
                    if not line:
                        empty_count += 1
                        continue
                    else:
                        empty_count = 0
                        last_data_time = time.time()
                    
                    # Parse <SDATA>HEXDATA</> format
                    if line.startswith("<SDATA>") and line.endswith("</>"):
                        hex_chunk = line[7:-3]  # Extract hex data
                        chunk_count += 1
                        
                        # Validate hex data (only 0-9, A-F)
                        if not all(c in '0123456789ABCDEF' for c in hex_chunk):
                            print(f"\nWarning: Invalid hex data in chunk {chunk_count}: {hex_chunk[:20]}...")
                            continue
                        
                        # Convert hex chunk to bytes
                        for i in range(0, len(hex_chunk), 2):
                            if i + 1 < len(hex_chunk):
                                byte_val = int(hex_chunk[i:i+2], 16)
                                file_data.append(byte_val)
                        
                        # Show progress every second
                        now = time.time()
                        if now - last_update >= 1.0:
                            progress = int((len(file_data) / size) * 100)
                            print(f"Progress: {progress}% ({len(file_data)}/{size} bytes, {chunk_count} chunks)")
                            last_update = now
                
                print(f"\nReceived {len(file_data)} bytes in {chunk_count} chunks")
                break
                
            elif line == "ERROR: File not found":
                print("Screenshot not found. Wait 30s after WiFi connects.")
                return False
                
        ser.close()
        
        if file_data:
            with open('dashboard.bmp', 'wb') as f:
                f.write(file_data)
            print(f"✓ Saved dashboard.bmp ({len(file_data)} bytes)")
            return True
        else:
            print("No data received")
            return False
            
    except serial.SerialException as e:
        print(f"Error: {e}")
        print("Make sure the ESP32 is connected and the port is correct")
        return False
    except KeyboardInterrupt:
        print("\nAborted by user")
        return False

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Download screenshot from ESP32 via serial')
    parser.add_argument('-s', '--screenshot', action='store_true', 
                        help='Take a screenshot before downloading')
    parser.add_argument('port', nargs='?', default='/dev/cu.usbserial-10',
                        help='Serial port (default: /dev/cu.usbserial-10)')
    parser.add_argument('baudrate', nargs='?', type=int, default=115200,
                        help='Baud rate (default: 115200)')
    
    args = parser.parse_args()
    
    download_screenshot(args.port, args.baudrate, args.screenshot)
