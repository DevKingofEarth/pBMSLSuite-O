#!/bin/bash
# pBMSLSuite-O Compilation Script
# Usage: ./compile.sh

cd "$(dirname "$0")"

echo "🔧 Compiling pBMSLSuite-O for ESP32..."

nix-shell -p python3 python3.pkgs.pyserial --run "arduino-cli compile -b esp32:esp32:esp32 --output-dir build"

if [ $? -eq 0 ]; then
    echo "✅ Compilation successful!"
    echo "📁 Output files:"
    ls -lh build/*.bin build/*.elf 2>/dev/null | awk '{print "   " $9, " (" $5 ")"}'
    echo ""
    echo "🚀 Ready for Wokwi simulation or ESP32 flash"
else
    echo "❌ Compilation failed!"
    exit 1
fi
