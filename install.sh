#!/usr/bin/env bash

set -e

INSTALL_PATH="/usr/local/bin/marex"

OS="$(uname)"
ARCH="$(uname -m)"

# Prevent running full script as root
if [ "$EUID" -eq 0 ]; then
    echo "Please do not run this script with sudo."
    echo "Run it normally: ./install.sh"
    exit 1
fi

clear

echo "Running $OS on $ARCH"
echo "====== Marex Installer ======"
echo ""
echo "1) Install Interpreter"
echo "2) Uninstall Interpreter"
echo "3) Exit"
echo ""
read -p "Choose an option: " OPTION
echo ""

case $OPTION in

1)
    echo "Checking for g++..."

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

    echo "Requesting sudo access..."
    sudo -v

    echo "Installing to /usr/local/bin..."
    sudo cp "$BUILD_DIR/marex" "$INSTALL_PATH"
    sudo chmod +x "$INSTALL_PATH"

    rm -rf "$BUILD_DIR"

    echo ""
    echo "Marex installed successfully."
    echo "Run it anywhere using: marex"
    ;;

2)
    if [ -f "$INSTALL_PATH" ]; then
        echo "Requesting sudo access..."
        sudo -v

        sudo rm "$INSTALL_PATH"

        echo "Marex has been uninstalled."
    else
        echo "Marex is not installed."
    fi
    ;;

3)
    echo "Exiting."
    exit 0
    ;;

*)
    echo "Invalid option."
    exit 1
    ;;

esac
