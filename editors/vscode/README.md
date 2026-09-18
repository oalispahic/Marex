# Marex for Visual Studio Code

Language support for [Marex](../../README.md) `.mx` files:

- syntax highlighting (keywords, strings, numbers, comments, function names,
  `ARGC`/`ARGV*`, operators)
- a formatter (`Format Document`, or format on save) that re-indents
  `if/fi`, `loop/done`, `fun/ret` and `else` blocks and trims whitespace
- comment toggling, bracket matching and auto-closing pairs
- snippets: `if`, `ifelse`, `loop`, `range`, `rangestep`, `fun`, `funv`, `println`

## Install

From this directory:

```sh
./install.sh            # links the extension into ~/.vscode/extensions
```

then reload VS Code (`Developer: Reload Window`). Because it is a link, pulling
new commits updates the extension too. Remove it with `./install.sh uninstall`.

To build a `.vsix` instead:

```sh
npx @vscode/vsce package
code --install-extension marex-lang-*.vsix
```

## Formatting

The formatter changes only whitespace. Indentation follows the block structure:

```
fun sign(n)
    if (n < 0) ret -1 fi     // early return, stays inside the if
    if (n > 0)
        ret 1
    fi
ret 0                        // closes the function, back at column 0
```

Settings:

- `marex.format.collapseBlankLines` (default `true`): collapse runs of blank
  lines into one.
- Indentation width and tabs vs. spaces follow the editor's own settings for
  the document.

Enable format on save for Marex only:

```json
"[marex]": {
  "editor.formatOnSave": true
}
```

## Development

```sh
node --test test/     # formatter unit tests, no dependencies needed
```
