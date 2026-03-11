#!/usr/bin/env bash

set -e

INSTALL_DIR="/usr/local/bin"
INSTALL_PATH="$INSTALL_DIR/marex"

OS=""$(uname)""
ARCH=""$(uname -m)""
# Resolve the directory containing this script so compilation works regardless
# of where the user invokes the script from.
# ${BASH_SOURCE[0]} works in bash; ${(%):-%x} is the zsh equivalent.
if [ -n "$BASH_SOURCE" ]; then
    SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]})")" && pwd)"
else
    SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
fi

# Prevent running full script as root
if [ "$(id -u)" -eq 0 ]; then
    echo "Please do not run this script with sudo."
    echo "Run it normally: ./install.sh"
    exit 1
fi

# Detect C++ compiler: prefer g++, fall back to clang++
detect_compiler() {
    if command -v g++ > /dev/null 2>&1; then
        echo "g++"
    elif command -v clang++ > /dev/null 2>&1; then
        echo "clang++"
    else
        echo ""
    fi
}

# Collect all marex binaries found on $PATH into the FOUND_BINS array.
# Works on Bash 3.2+ and zsh (no mapfile needed).
collect_marex_on_path() {
    FOUND_BINS=()
    local dir
    local saved_IFS="$IFS"
    IFS=":"
    for dir in $PATH; do
        IFS="$saved_IFS"
        if [ -x "$dir/marex" ]; then
            FOUND_BINS+=("$dir/marex")
        fi
    done
    IFS="$saved_IFS"
}

clear

echo "Running $OS on $ARCH"
echo "====== Marex Installer ======"
echo ""
echo "1) Install Interpreter"
echo "2) Uninstall Interpreter"
echo "3) Exit"
echo ""
printf "Choose an option: "
read -r OPTION
echo ""

case $OPTION in

1)
    # Detect compiler
    CXX="$(detect_compiler)"
    if [ -z "$CXX" ]; then
        echo "Error: No C++ compiler found. Please install g++ or clang++."
        exit 1
    fi
    echo "Using compiler: $CXX ($(command -v "$CXX"))"

    # Detect and remove any stale marex binaries on $PATH before installing
    collect_marex_on_path
    if [ "{#FOUND_BINS[@]}" -gt 0 ]; then
        echo ""
        echo "Warning: Found existing marex binary/binaries on \$PATH:"
        for bin in "${FOUND_BINS[@]}"; do
            echo "  $bin"
        done
        echo "Removing them before installing to avoid shadowing..."
        echo "Requesting sudo access to remove stale binaries..."
        sudo -v
        for bin in "${FOUND_BINS[@]}"; do
            sudo rm -f "$bin"
            echo "  Removed: $bin"
        done
        echo ""
    fi

    echo "Compiling Marex..."

    BUILD_DIR="$(mktemp -d /tmp/build_marex_install.XXXXXX)"

    # -w suppresses all compiler warnings for a clean install output
    "$CXX" -std=c++14 -O2 -w "$SCRIPT_DIR"/src/*.cpp -I "$SCRIPT_DIR/include" -o "$BUILD_DIR/marex"

    echo "Requesting sudo access to install..."
    sudo -v

    # Create /usr/local/bin if it doesn't exist (may be absent on fresh macOS)
    if [ ! -d "$INSTALL_DIR" ]; then
        echo "Creating $INSTALL_DIR..."
        sudo mkdir -p "$INSTALL_DIR"
    fi

    echo "Installing to $INSTALL_PATH..."
    sudo cp "$BUILD_DIR/marex" "$INSTALL_PATH"
    sudo chmod +x "$INSTALL_PATH"

    rm -rf "$BUILD_DIR"

    echo ""
    echo "Marex installed successfully."
    echo "  Compiler : $CXX"
    echo "  Installed: $INSTALL_PATH"
    echo ""

    # Verify the installed binary is the one that will run
    ACTIVE_BIN="$(command -v marex 2>/dev/null || true)"
    if [ -z "$ACTIVE_BIN" ]; then
        echo "Warning: 'marex' is not yet found on \$PATH."
        echo "Make sure $INSTALL_DIR is in your \$PATH."
    elif [ "$ACTIVE_BIN" != "$INSTALL_PATH" ]; then
        echo "Warning: 'marex' resolves to '$ACTIVE_BIN', not '$INSTALL_PATH'."
        echo "Something is still shadowing the newly installed binary."
        echo "Check your \$PATH order."
    else
        echo "Verified: 'marex' resolves to $INSTALL_PATH"
        echo "Run it anywhere using: marex"
    fi
    ;;

2)
    # Find and remove ALL marex binaries anywhere on $PATH
    collect_marex_on_path

    if [ "{#FOUND_BINS[@]}" -eq 0 ]; then
        echo "Marex is not installed (no 'marex' binary found on \$PATH)."
    else
        echo "Found the following marex binary/binaries:"
        for bin in "${FOUND_BINS[@]}"; do
            echo "  $bin"
        done
        echo ""
        echo "Requesting sudo access to remove..."
        sudo -v
        for bin in "${FOUND_BINS[@]}"; do
            sudo rm -f "$bin"
            echo "  Removed: $bin"
        done
        echo ""
        echo "Marex has been uninstalled."
    fi
    ;;

3)
    echo "Exiting."
    exit 0
    ;;

*)
    echo "Invalid option."
    exit 1;
    ;;

esac
