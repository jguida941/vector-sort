#!/bin/bash
set -euo pipefail

# Change to the project root (one level up from scripts/) so relative paths work
cd "$(dirname "$0")/.."

echo "==> Checking prerequisites..."

if ! command -v cmake >/dev/null 2>&1; then
  # Try to show a friendly dialog if GUI is available; fall back to console
  osascript -e 'display dialog "CMake not found. Install with: brew install cmake" buttons {"OK"} default button 1' >/dev/null 2>&1 || true
  echo "ERROR: CMake not found. Install with Homebrew: brew install cmake"
  exit 1
fi

# Xcode Command Line Tools provide clang/c++
if ! xcode-select -p >/dev/null 2>&1; then
  osascript -e 'display dialog "Xcode Command Line Tools not found. Run: xcode-select --install" buttons {"OK"} default button 1' >/dev/null 2>&1 || true
  echo "ERROR: Xcode Command Line Tools not found. Run: xcode-select --install"
  exit 1
fi

echo "==> Building (CMake + Ninja/Make)..."
mkdir -p build
cmake -S . -B build
cmake --build build -j || {
  echo "Build failed. See messages above."; read -rp "Press Return to close..." _; exit 1;
}

# Default CSV in repo (now in data/ folder)
CSV="data/eBid_Monthly_Sales.csv"

# Offer to choose a CSV via AppleScript dialog (if available)
CHOICE=$(osascript <<'APPLESCRIPT'
try
set theButton to button returned of (display dialog "Run with default CSV (data/eBid_Monthly_Sales.csv)?" buttons {"Choose CSV…","Default"} default button "Default")
if theButton is "Default" then
  return ""
else
  set theFile to choose file with prompt "Select a CSV file" of type {"public.comma-separated-values-text", "csv"}
  return POSIX path of theFile
end if
on error
return "ERROR_NO_UI"
end try
APPLESCRIPT
)

if [ "$CHOICE" = "ERROR_NO_UI" ]; then
  echo "No GUI available; running with default CSV."
elif [ -n "$CHOICE" ]; then
  CSV="$CHOICE"
fi

echo "==> Launching ./build/Vector_Sort \"$CSV\""
./build/Vector_Sort "$CSV" || true

echo
read -rp "Press Return to close..." _

