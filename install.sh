#!/usr/bin/env bash
#
# Marex installer for macOS and Linux.
#
#   ./install.sh                     interactive menu
#   ./install.sh install             build and install to /usr/local/bin
#   ./install.sh install --prefix ~/.local
#   ./install.sh uninstall
#   ./install.sh --help
#
# Builds with CMake when it is available, otherwise compiles the sources
# directly with g++ or clang++. Uses sudo only if the target directory is
# not writable by the current user.

set -e

PREFIX="/usr/local"
MODE=""
ASSUME_YES=0
USE_CMAKE="auto"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]:-$0}")" && pwd)"

usage() {
    cat <<USAGE
Usage: ./install.sh [install|uninstall] [options]

Commands:
  install          Build Marex and install the 'marex' binary
  uninstall        Remove the installed 'marex' binary
  (none)           Show an interactive menu

Options:
  --prefix DIR     Install under DIR/bin (default: $PREFIX)
  --no-cmake       Compile the sources directly even if CMake is installed
  -y, --yes        Never prompt (other copies of marex on PATH are left alone)
  -h, --help       Show this message
USAGE
}

log()  { printf '%s\n' "$*"; }
warn() { printf 'Warning: %s\n' "$*" >&2; }
die()  { printf 'Error: %s\n' "$*" >&2; exit 1; }

# ---------------------------------------------------------------- helpers

# Nearest existing ancestor of a path (the path itself if it exists).
existing_ancestor() {
    local dir="$1"
    while [ ! -e "$dir" ] && [ "$dir" != "/" ] && [ -n "$dir" ]; do
        dir="$(dirname "$dir")"
    done
    printf '%s' "$dir"
}

# Runs a command with sudo only when the target directory (or, if it does
# not exist yet, its nearest existing ancestor) is not writable.
as_owner_of() {
    local dir
    dir="$(existing_ancestor "$1")"; shift
    if [ -w "$dir" ]; then
        "$@"
    else
        if [ "$SUDO_NOTICE" != "1" ]; then
            log "Requesting administrator access to write to $dir"
            SUDO_NOTICE=1
        fi
        sudo "$@"
    fi
}

# Asks a yes/no question. Non-interactive runs (-y) always answer "no"
# here: this is only used before deleting files the installer did not
# create, which should never happen without a person confirming it.
confirm() {
    if [ "$ASSUME_YES" -eq 1 ]; then return 1; fi
    printf '%s [y/N] ' "$1"
    read -r answer
    case "$answer" in
        y|Y|yes|YES) return 0 ;;
        *) return 1 ;;
    esac
}

detect_compiler() {
    if command -v g++ > /dev/null 2>&1; then
        echo "g++"
    elif command -v clang++ > /dev/null 2>&1; then
        echo "clang++"
    else
        echo ""
    fi
}

project_version() {
    sed -nE 's/^project\(Marex VERSION ([0-9]+\.[0-9]+\.[0-9]+).*/\1/p' "$SCRIPT_DIR/CMakeLists.txt"
}

# Fills FOUND_BINS with every 'marex' on $PATH that is not $INSTALL_PATH.
# Works on bash 3.2 (macOS) - no mapfile.
collect_other_marex_on_path() {
    FOUND_BINS=()
    local dir
    local saved_IFS="$IFS"
    IFS=":"
    for dir in $PATH; do
        IFS="$saved_IFS"
        [ -z "$dir" ] && continue
        if [ -x "$dir/marex" ] && [ "$dir/marex" != "$INSTALL_PATH" ]; then
            FOUND_BINS+=("$dir/marex")
        fi
    done
    IFS="$saved_IFS"
}

# ------------------------------------------------------------------ build

build_with_cmake() {
    log "Building with CMake..."
    cmake -S "$SCRIPT_DIR" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release > "$BUILD_DIR/configure.log" 2>&1 \
        || { cat "$BUILD_DIR/configure.log"; die "CMake configuration failed"; }
    cmake --build "$BUILD_DIR" > "$BUILD_DIR/build.log" 2>&1 \
        || { cat "$BUILD_DIR/build.log"; die "Build failed"; }
    BUILT_BIN="$BUILD_DIR/marex"
}

build_with_compiler() {
    local cxx
    cxx="$(detect_compiler)"
    [ -n "$cxx" ] || die "No C++ compiler found. Install g++ or clang++ (or CMake)."
    log "Building with $cxx ($(command -v "$cxx"))..."

    # Generate version.hpp the same way CMake's configure_file does.
    local version major minor patch
    version="$(project_version)"
    [ -n "$version" ] || die "Could not read the project version from CMakeLists.txt"
    major="${version%%.*}"
    patch="${version##*.}"
    minor="${version#*.}"; minor="${minor%.*}"

    mkdir -p "$BUILD_DIR/generated"
    sed -e "s/@PROJECT_VERSION_MAJOR@/$major/" \
        -e "s/@PROJECT_VERSION_MINOR@/$minor/" \
        -e "s/@PROJECT_VERSION_PATCH@/$patch/" \
        -e "s/@PROJECT_VERSION@/$version/" \
        "$SCRIPT_DIR/include/version.hpp.in" > "$BUILD_DIR/generated/version.hpp"

    "$cxx" -std=c++14 -O2 \
        -I "$SCRIPT_DIR/include" -I "$BUILD_DIR/generated" \
        "$SCRIPT_DIR"/src/*.cpp -o "$BUILD_DIR/marex" \
        || die "Compilation failed"
    BUILT_BIN="$BUILD_DIR/marex"
}

do_install() {
    INSTALL_DIR="$PREFIX/bin"
    INSTALL_PATH="$INSTALL_DIR/marex"

    log "Marex $(project_version) -> $INSTALL_PATH"
    log ""

    BUILD_DIR="$(mktemp -d "${TMPDIR:-/tmp}/marex-build.XXXXXX")"
    trap 'rm -rf "$BUILD_DIR"' EXIT

    if [ "$USE_CMAKE" != "no" ] && command -v cmake > /dev/null 2>&1; then
        build_with_cmake
    else
        build_with_compiler
    fi

    log "Installing to $INSTALL_PATH..."
    if [ ! -d "$INSTALL_DIR" ]; then
        as_owner_of "$INSTALL_DIR" mkdir -p "$INSTALL_DIR"
    fi
    as_owner_of "$INSTALL_DIR" cp "$BUILT_BIN" "$INSTALL_PATH"
    as_owner_of "$INSTALL_DIR" chmod 755 "$INSTALL_PATH"

    log ""
    log "Installed: $INSTALL_PATH ($("$INSTALL_PATH" --version))"

    # Other copies earlier on $PATH would shadow the one just installed.
    collect_other_marex_on_path
    if [ "${#FOUND_BINS[@]}" -gt 0 ]; then
        log ""
        warn "other 'marex' binaries exist on your PATH:"
        for bin in "${FOUND_BINS[@]}"; do log "  $bin"; done
        if confirm "Remove them so $INSTALL_PATH is the one that runs?"; then
            for bin in "${FOUND_BINS[@]}"; do
                as_owner_of "$(dirname "$bin")" rm -f "$bin"
                log "  Removed $bin"
            done
        else
            log "Leaving them in place."
        fi
    fi

    log ""
    local active
    active="$(command -v marex 2>/dev/null || true)"
    if [ -z "$active" ]; then
        warn "'marex' is not on your PATH yet. Add this to your shell profile:"
        log "  export PATH=\"$INSTALL_DIR:\$PATH\""
    elif [ "$active" != "$INSTALL_PATH" ]; then
        warn "'marex' currently resolves to $active, not $INSTALL_PATH. Check your PATH order."
    else
        log "Done. Run 'marex' for the REPL or 'marex program.mx' to run a script."
    fi
}

do_uninstall() {
    INSTALL_DIR="$PREFIX/bin"
    INSTALL_PATH="$INSTALL_DIR/marex"

    local removed=0
    if [ -e "$INSTALL_PATH" ]; then
        as_owner_of "$INSTALL_DIR" rm -f "$INSTALL_PATH"
        log "Removed $INSTALL_PATH"
        removed=1
    fi

    collect_other_marex_on_path
    if [ "${#FOUND_BINS[@]}" -gt 0 ]; then
        log "Other 'marex' binaries found on your PATH:"
        for bin in "${FOUND_BINS[@]}"; do log "  $bin"; done
        if confirm "Remove these too?"; then
            for bin in "${FOUND_BINS[@]}"; do
                as_owner_of "$(dirname "$bin")" rm -f "$bin"
                log "  Removed $bin"
            done
            removed=1
        else
            log "Leaving them in place."
        fi
    fi

    if [ "$removed" -eq 1 ]; then
        log "Marex has been uninstalled."
    else
        log "Marex is not installed under $PREFIX (and no other copy is on your PATH)."
    fi
}

menu() {
    log "====== Marex Installer ======"
    log "$(uname) $(uname -m), prefix $PREFIX"
    log ""
    log "1) Install"
    log "2) Uninstall"
    log "3) Exit"
    log ""
    printf 'Choose an option: '
    read -r option
    log ""
    case "$option" in
        1) do_install ;;
        2) do_uninstall ;;
        3) exit 0 ;;
        *) die "Invalid option." ;;
    esac
}

# --------------------------------------------------------------- arguments

while [ $# -gt 0 ]; do
    case "$1" in
        install|uninstall)
            [ -z "$MODE" ] || die "Only one of install/uninstall may be given"
            MODE="$1" ;;
        --prefix)
            [ $# -ge 2 ] || die "--prefix needs a directory"
            PREFIX="$2"; shift ;;
        --prefix=*) PREFIX="${1#--prefix=}" ;;
        --no-cmake) USE_CMAKE="no" ;;
        -y|--yes) ASSUME_YES=1 ;;
        -h|--help) usage; exit 0 ;;
        *) usage >&2; die "Unknown argument: $1" ;;
    esac
    shift
done

# Expand a leading ~ that was quoted, and make the prefix absolute.
case "$PREFIX" in
    "~"|"~/"*) PREFIX="$HOME${PREFIX#\~}" ;;
esac

if [ "$(id -u)" -eq 0 ] && [ -z "$SUDO_USER" ] && [ "$ASSUME_YES" -eq 0 ]; then
    warn "running as root; the binary will be owned by root."
fi

case "$MODE" in
    install)   do_install ;;
    uninstall) do_uninstall ;;
    "")        menu ;;
esac
