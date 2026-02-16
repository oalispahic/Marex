#!/usr/bin/env bash

set -e

echo "=== Marex Installer ==="

# Detect OS
OS="$(uname)"
ARCH="$(uname -m)"

if [[ "$OS" != "Darwin" && "$OS" != "Linux" ]]; then
    echo "Unsupported OS"
    exit 1
fi

# Check for compiler
if ! command -v g++ &> /dev/null; then
    echo "Error: g++ is required but not installed."
    exit 1
fi

echo "Compiling Marex..."

BUILD_DIR="build_marex_install"
rm -rf "$BUILD_DIR"
mkdir "$BUILD_DIR"
cd "$BUILD_DIR"

g++ -std=c++11 -O2 ../src/*.cpp -I../include -o marex

cd ..

echo "Installing to /usr/local/bin"

sudo cp "$BUILD_DIR/marex" /usr/local/bin/marex
sudo chmod +x /usr/local/bin/marex

rm -rf "$BUILD_DIR"

echo ""
echo "Marex installed successfully."
echo "Run by typing marex in terminal from anywhere"
