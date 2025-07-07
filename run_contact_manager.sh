#!/bin/bash

# Contact Manager Launcher Script
# This script compiles and runs the contact manager

# Get the directory where this script is located
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# Check if GTK4 is available
if ! pkg-config --exists gtk4; then
    echo "Error: GTK4 is not installed on this system."
    echo "Please install GTK4 development libraries."
    read -p "Press Enter to exit..."
    exit 1
fi

# Compile the program
echo "Compiling Contact Manager..."
gcc $(pkg-config --cflags gtk4) -o contact_manager main.c $(pkg-config --libs gtk4)

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful! Starting Contact Manager..."
    # Run the program
    ./contact_manager
else
    echo "Compilation failed!"
    read -p "Press Enter to exit..."
    exit 1
fi
