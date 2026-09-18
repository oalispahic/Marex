# Marex

**A tiny high-level, imperative, interpreted, dynamically typed language built in C++**

It started as a university project and an exercise in language design: a formal
grammar written in EBNF, then a lexer, an LL(k) parser, an AST and a tree-walking
interpreter, all written from scratch with no dependencies beyond the C++14
standard library. It is meant to be small and readable while still supporting real
control flow and non-trivial programs.

```
var last_pass := 1

loop (var i := 0; i < 5; i++)
    loop (var j := 0; j < 5; j++)
        if (j > i)
            print("* ")
        fi
    done

    if (i = 4)
        last_pass := 0
    fi

    if (last_pass)
        print(newln)
    fi
done
```
Output:
```
* * * *
* * *
* *
*
```

---

## Installation

Requires a C++14 compiler (g++ or clang++). CMake 3.16+ is used when present.

```sh
git clone https://github.com/oalispahic/Marex.git
cd Marex
./install.sh install            # installs /usr/local/bin/marex
```

Other ways to run the installer:

```sh
./install.sh                            # interactive menu
./install.sh install --prefix ~/.local  # install to ~/.local/bin, no sudo needed
./install.sh install --no-cmake         # compile directly with g++/clang++
./install.sh uninstall
```

The installer asks for administrator access only if the target directory is not
writable, and warns if another `marex` earlier on your `PATH` would shadow the
new one.

### Building by hand

```sh
cmake -S . -B build
cmake --build build
./build/marex --version
ctest --test-dir build        # runs the example programs and CLI checks
cmake --install build         # optional, honours CMAKE_INSTALL_PREFIX
```

---

## Usage

```
marex                      start the REPL
marex program.mx           run a script
marex program.mx a b c     run a script with arguments
marex --tokens program.mx  print the token stream instead of running
marex --help
marex --version
```

Script arguments are visible inside the program as `ARGC` and `ARGV0`, `ARGV1`, ...

Exit status is `0` on success, `1` for a syntax or runtime error in the script and
`2` for a bad command line or a file that cannot be opened.

### REPL

Running `marex` without a file starts an interactive session. Statements run as
soon as they are complete, so an `if` or `loop` keeps prompting with `...` until
its closing `fi` or `done`. Variables persist between inputs.

| Command  | Effect                                                     |
|----------|------------------------------------------------------------|
| `:help`  | List the commands                                          |
| `:info`  | Show defined functions and every variable with its type, value and memory use |
| `:reset` | Forget all variables                                       |
| `:clear` | Clear the screen                                           |
| `:exit`  | Leave (also Ctrl-D)                                        |

Up and down arrows walk the line history and Ctrl-C cancels the statement being
typed.

---

## Language tour

```
// Line comments, and /* block comments */ that may span lines.

var n := 6            // int
var half := 2.5       // float
var name := "Marex"   // string
var later             // no value yet: NaN

n := n + 1            // assignment uses :=
n++                   // shorthand for n := n + 1

print(name + " " + n) // strings and numbers join into strings
print(newln)          // newln is the newline character
```

**Types.** Values are `int`, `float`, `string` or `NaN`. Arithmetic on two ints
stays an int (so `7 / 2` is `3`); if either side is a float the result is a float.
Mixing a string with anything but `+` is an error, as is division by zero.

**Conditions.** `=` and `!=` test equality (`=` is comparison, `:=` is assignment),
`<` and `>` compare numbers or strings, `&` and `|` are logical and/or. There is no
boolean type: comparisons produce `1` or `0`, and any non-zero number or non-empty
string counts as true.

```
if (n > 5 & name = "Marex")
    print("yes")
else
    print("no")
fi
```

**Loops.** A C-style loop, or a range loop whose direction is inferred from its
bounds. The parentheses around a range header are optional.

```
loop (var i := 0; i < 3; i++)   // 0 1 2
    print(i)
done

loop 1 -> 4          // runs for 1, 2, 3 (end is exclusive)
    print("up")
done

loop (10 -> 0 step -4)   // 10, 6, 2
    print("down")
done
```

**Functions.** `fun` opens a function, `ret` closes it. A value written after
`ret` on the same line is returned; `ret` on its own makes the function void
(calling it yields `NaN`). Inside an `if` or `loop` within the body, `ret`
returns early.

```
fun fib(n)
    if (n < 2) ret n fi
ret fib(n - 1) + fib(n - 2)

fun greet(name)
    print("Hello, " + name)
    print(newln)
ret

greet("Marex")
print(fib(20))
```

Every call gets its own stack frame: parameters and `var` declarations inside
the body are local to that call, while globals stay readable and assignable.
Calling an undefined function, passing the wrong number of arguments, or
recursing deeper than 1000 calls is a runtime error, reported with a call trace.

**Shell commands.** `sys("ls -l")` runs a command through the system shell.

More examples live in [`test_program/`](test_program/).

---

## Architecture

```
    Source file (.mx)
           ↓
         Lexer        src/lexer.cpp        text → tokens
           ↓
         Parser       src/parser.cpp       tokens → AST (include/ast_nodes.hpp)
           ↓
      Interpreter     src/interpreter.cpp  walks the AST, values in values_types.cpp
```

`src/cli.cpp` parses the command line, `src/runtime.cpp` runs a whole file and
`src/repl.cpp` is the interactive loop. The formal grammar is in
[`grammar/grammar.ebnf`](grammar/grammar.ebnf).

Syntax errors are reported during parsing with a line number; runtime errors
(undefined variables, type mismatches, division by zero) stop execution with a
message.

---

## License

This project is licensed under the [MIT License](LICENSE).
