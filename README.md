This project is licensed under the [MIT License](LICENSE).
# Marex

**A tiny high-level, imperative, interpreted, dynamically typed language built in C++**

It was initially developed as a university project and is designed to be simple and educational, while still supporting real control flow and non-trivial programs.

This repository containt the first stable version.

---
### About:

It started as an exercise in language design and implementation, beginning with a formal grammar
written in EBNF form. 

The following implementation consists of: 
* Lexer
* LL1 parser
* AST tree
* Tree walking interpreter


---
### Overview:
Bellow is a simple example program demonstrating core functionalities
like nested loops, conditionals, dynamic typing and a formatted output

```cpp
new last_pass := 1

loop(new i := 0; i<5; i := i+1)
    loop(new j := 0; j<5; j:=j+1)
        if(j > i)
            print("* ")
        fi
    done
    
    if(i = 4)
        last_pass := 0
    fi

    if(last_pass)
        print(newln)
    fi

done
i := "Test"
print(i)
```
Output:
```
* * * * 
* * * 
* *
*
Test
```
---
### Features  

* High-level and interpreted
* Structured control flow
* Dynamically typed variables 
* Two types of loops
  * classic C type
  * range based loop with some syntax candy with '->'
* Readable easy to understand
---
### Architecture 

The language follows an interpreter pipeline:

```
    Source file (.mx or .txt)
             ↓
           Lexer
             ↓
           Parser
             ↓
            AST
             ↓
         Interpreter
```

* The lexer turns the raw soruce file into an array of Tokens
* The parser is an LL1 parser that constructs an Abstract Syntax Tree (AST)
* Interpreter walks the AST and executes directly

---

### Formal grammar

The language is defined using EBNF and can be used with bison 

---

### Error handling

Syntax errors are handled during parsing and runtime errors are reported during execution

---

### Compile and try

If you would like to try to compile the project, clone this repo and if you have g++ installed run 
```
g++ -std=c++11 src/*.cpp -Iinclude -o marex
```
in the root of your directory. You will get a marex compile.

Then you can write a program or try the example.md in the test_program directory.

Run by doing ./marex "test_program/example.md" 

Marex also supports a REPL mode which you get by running marex with no args like
./marex
