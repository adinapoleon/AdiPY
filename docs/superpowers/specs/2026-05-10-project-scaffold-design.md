# Project Scaffold and Lexer Design

## Goal

Create the first working slice of the Python-inspired JIT project: a Makefile-driven C++ layout, a CLI entry point, and a handwritten lexer that implements the token rules listed in the README. This slice should make it easy to run the lexer against a source file and to add parser work next.

## Scope

This design covers only the initial project structure, build commands, CLI shape, token model, lexer behavior, and first tests. It does not include parsing, AST construction, semantic analysis, IR, interpretation, JIT code generation, or runtime/GC implementation.

## File Structure

The project will use a conventional small C++ layout:

```text
.
├── Makefile
├── README.md
├── include/
│   └── adipy/
│       ├── source_location.hpp
│       ├── token.hpp
│       └── lexer.hpp
├── src/
│   ├── cli/
│   │   └── main.cpp
│   └── frontend/
│       └── lexer.cpp
└── tests/
    ├── test_main.cpp
    └── lexer_tests.cpp
```

Build artifacts will live under `build/`, and the initial scaffold should add a `.gitignore` entry for that directory.

## Build

Use a hand-written `Makefile` rather than CMake. The first targets should be:

- `make`: build the CLI binary at `build/adipy`
- `make test`: build and run a lightweight test binary
- `make clean`: remove build outputs

Compiler defaults should favor correctness and useful diagnostics:

```text
CXX ?= g++
CXXFLAGS ?= -std=c++20 -Wall -Wextra -Wpedantic -g
```

## CLI

The initial CLI command should be:

```text
adipy lex <path>
```

It reads the provided source file, tokenizes it, and prints one token per line with kind, lexeme, line, and column. This keeps the CLI useful immediately and creates a simple manual debugging loop before parser work begins.

Expected first-run behavior:

- missing arguments print usage and return a nonzero status
- unknown commands print usage and return a nonzero status
- unreadable files print an error and return a nonzero status
- lexer diagnostics print a location and return a nonzero status

## Lexer

Although the README lists token regexes, the implementation should be a handwritten scanner rather than `std::regex`. This keeps longest-match behavior explicit for operators such as `<=`, `>=`, `==`, `!=`, `&&`, and `||`, and gives direct control over line and column tracking.

The lexer should recognize:

- whitespace and `//` line comments as skipped trivia
- identifiers and reserved keywords
- integer literals matching `0|[1-9][0-9]*`
- punctuation: `(`, `)`, `{`, `}`, `,`, `;`, `:`, `.`
- operators: `+`, `-`, `*`, `/`, `%`, `=`, `==`, `!=`, `<`, `<=`, `>`, `>=`, `&&`, `||`, `!`

Reserved keywords are:

```text
class extends if else while return break continue
true false int bool void this super null
```

The README's class syntax examples use `class Point : Object`, while the grammar sketch uses `extends`. For the lexer, both `:` and `extends` are tokenized; the parser can decide which class inheritance spelling to accept when that phase starts.

## Token Model

Tokens should include:

- `TokenKind`
- lexeme text
- source location containing line and column

The lexer should append an explicit end-of-file token so parser code can consume streams predictably later.

## Error Handling

Lexer errors should preserve the first invalid character location and a concise message. The first implementation can stop at the first lexical error rather than attempting recovery.

Examples:

- unexpected character
- standalone `&`
- standalone `|`
- invalid integer literal if a leading-zero rule is violated by forms like `012`

## Tests

Use a tiny in-repo test harness for now instead of bringing in a framework. Initial tests should cover:

- keywords versus identifiers
- comments and whitespace skipping
- all single-character punctuation/operators
- all two-character operators
- source line and column tracking
- invalid character errors
- integer literal rules

## Next Step After This Slice

Once this foundation is working, parser work can begin with recursive descent declarations/statements and a Pratt expression parser, using the lexer output and source locations defined here.
