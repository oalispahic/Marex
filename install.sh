#!/usr/bin/env bash

set -e  # Exit on error

echo "=== Marex Installer ==="

# Detect OS
OS="$(uname)"
ARCH="$(uname -m)"

echo "Detected system: $OS ($ARCH)"

if [[ "$OS" != "Darwin" && "$OS" != "Linux" ]]; then
    echo "Unsupported operating system."
    exit 1
fi

# Check for g++
if ! command -v g++ &> /dev/null; then
    echo "Error: g++ is not installed."
    echo "Please install a C++ compiler and try again."
    exit 1
fi

echo "Compiler found: $(g++ --version | head -n 1)"

# Create temporary build directory
BUILD_DIR="build_marex_install"
rm -rf "$BUILD_DIR"
mkdir "$BUILD_DIR"
cd "$BUILD_DIR"

echo "Compiling Marex..."

# Adjust this if your file layout changes
g++ -std=c+11 -O2 ../src/*.cpp -I../include -o marex

echo "Compilation successful."

# Install location
INSTALL_DIR="$HOME/.local/bin"
mkdir -p "$INSTALL_DIR"

cp marex "$INSTALL_DIR/"
chmod +x "$INSTALL_DIR/marex"

echo ""
echo "Installed Marex to:"
echo "  $INSTALL_DIR/marex"

# Clean up
cd ..
rm -rf "$BUILD_DIR"

# Check if ~/.local/bin is in PATH
if [[ ":$PATH:" != *":$INSTALL_DIR:"* ]]; then
    echo ""
    echo "IMPORTANT:"
    echo "Add this line to your ~/.bashrc or ~/.zshrc:"
    echo ""
    echo "  export PATH=\"$INSTALL_DIR:\$PATH\""
    echo ""
    echo "Then restart your terminal."
fi

echo ""
echo "Installation complete."
echo "You can now run:"
echo "  marex --help"
