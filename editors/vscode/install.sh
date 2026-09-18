#!/usr/bin/env bash
# Links (or unlinks) this extension into the VS Code extensions folder.
#   ./install.sh            install
#   ./install.sh uninstall  remove
set -e

HERE="$(cd "$(dirname "${BASH_SOURCE[0]:-$0}")" && pwd)"
EXT_DIR="${VSCODE_EXTENSIONS:-$HOME/.vscode/extensions}"
TARGET="$EXT_DIR/oalispahic.marex-lang"

case "${1:-install}" in
    install)
        mkdir -p "$EXT_DIR"
        if [ -e "$TARGET" ] && [ ! -L "$TARGET" ]; then
            echo "Error: $TARGET exists and is not a link; remove it first." >&2
            exit 1
        fi
        ln -sfn "$HERE" "$TARGET"
        echo "Linked $TARGET -> $HERE"
        echo "Reload VS Code (Developer: Reload Window) to activate it."
        ;;
    uninstall)
        if [ -L "$TARGET" ]; then
            rm "$TARGET"
            echo "Removed $TARGET"
        else
            echo "Not installed ($TARGET is not a link)."
        fi
        ;;
    *)
        echo "Usage: $0 [install|uninstall]" >&2
        exit 1
        ;;
esac
