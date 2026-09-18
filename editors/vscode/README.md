# Marex for Visual Studio Code

Language support for [Marex](../../README.md) `.mx` files:

- syntax highlighting (keywords, strings, numbers, comments, function names,
  `ARGC`/`ARGV*`, operators) and an "MX" file icon for `.mx` files
- **Marex: Run File** (play button in the editor title, right-click menu,
  or `Ctrl+Alt+R` / `Cmd+Alt+R`) runs the current file in the integrated
  terminal; **Marex: Open REPL** starts the interactive interpreter
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

## Running

`Marex: Run File` saves the file and runs `marex <file>` in a terminal named
"Marex", reusing it between runs. Settings:

- `marex.executablePath` (default `marex`): the interpreter to use. Set this
  if `marex` is not on the PATH that VS Code's terminal sees.
- `marex.run.arguments` (default `[]`): arguments passed to the script.
- `marex.run.saveBeforeRun` (default `true`).

The file icon is picked up by icon themes that show language icons, which
includes the built-in Seti theme.

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
