# C-Preprocessor-Mini-Project — `mypreprocessor`

A modular C-based systems-programming mini-project written from scratch to mimic core functionalities of the standard GNU C preprocessor (`cpp`). It demonstrates custom header/source modularity, lexical comment stripping, recursive file inclusion resolution, dynamic macro symbol-table management, line-by-line text expansion, and command-line orchestration.

## Project goals

- Parse C source files and process preprocessor directives from scratch.
- Strip single-line (`//`) and multi-line (`/* ... */`) comments while preserving code alignment.
- Resolve `#include` directives recursively for local headers and search directories.
- Maintain a dynamic symbol table for `#define` and `#undef` macro replacement.
- Evaluate conditional compilation blocks (`#ifdef`, `#ifndef`, `#endif`).
- Support custom include search paths via command-line `-I` options.
- Provide a clear, clean demonstration test suite using an optimized Makefile.

## Concepts demonstrated

Lexical parsing, string manipulation, symbol tables, file I/O, recursive file inclusion, search path resolution, macro substitution, conditional directives, header/source modularity, memory management, and GNU Make automation.

## Repository structure

```text
C-Preprocessor-Mini-Project/
├── Makefile
├── README.md
├── include/
│   ├── common.h
│   ├── utils.h
│   ├── macro.h
│   ├── comments.h
│   ├── includes.h
│   ├── expand.h
│   └── preprocessor.h
├── src/
│   ├── utils.c
│   ├── macro.c
│   ├── comments.c
│   ├── includes.c
│   ├── expand.c
│   ├── preprocessor.c
│   └── main.c
└── tests/
    ├── demo.h
    └── demo.c

```

## Requirements

* Linux (or WSL running a Linux distribution)
* GCC supporting the C11 standard


* GNU Make
* Standard Linux utilities (`cat`, `rm`, etc.)

## Build

```bash
make clean
make

```

The compiler flags enable common warnings and optimizations (`-Wall -Wextra -O2 -std=c11 -Iinclude`). A successful build finishes without errors.

## Run and inspect

Run the preprocessor against the provided demonstration test files (`tests/demo.c` and `tests/demo.h`):

```bash
make test

```

This command invokes the preprocessor, compiles `tests/demo.c`, generates the expanded output file `tests/demo.i`, and prints its contents directly to the terminal:

```bash
./mypreprocessor tests/demo.c -I. -v

```

## Usage

```bash
./mypreprocessor <input.c> [-I<dir>]... [-o output.i] [-v]

```

* `<input.c>`: Target C source file to preprocess.


* `-I<dir>`: Add a custom directory to the include file search path.


* `-o output.i`: Specify a custom output path for the expanded file.


* `-v`: Enable verbose logging and tracing mode.



## Module breakdown

| Module | Source Files | Purpose |
| --- | --- | --- |
| **Engine Core** | `src/main.c`, `src/preprocessor.c`<br> | Handles CLI arguments, stream orchestration, and pipeline control. |
| **Comment Stripper** | `src/comments.c`<br> | Safely strips single and multi-line comments from the input stream. |
| **Inclusion Engine** | `src/includes.c`<br> | Resolves `#include` directives and manages `-I` search paths and guards. |
| **Macro Manager** | `src/macro.c`<br> | Implements symbol table tracking for `#define` and `#undef`.

 |
| **Expansion Engine** | `src/expand.c`<br> | Performs text substitution and evaluates conditional directives (`#ifdef`).

 |
| **Utilities** | `src/utils.c`<br> | Provides safe memory allocation wrappers and string/token helpers. |

## Error handling & diagnostics

The preprocessor outputs informative diagnostics when encountering operational issues:

* Missing input files or unreadable paths (`cannot open %s`).


* Memory allocation exhaustion (`out of memory`).


* Output file write failures (`cannot open output file`).



## Learning outcomes

After studying this project, a developer should be able to explain token-based text replacement, implement a dynamic symbol table for macro parsing, manage recursive file inclusion paths, and orchestrate a multi-module C systems utility.

## License

MIT — see [LICENSE](https://www.google.com/search?q=LICENSE) (if applicable) or standard open-source guidelines.

```

```
