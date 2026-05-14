# Adipy

Adipy is a teaching-first JIT compiler project for a statically typed language with:

- C-style block syntax using `{}` and `()`
- newline-terminated statements instead of semicolons
- C-style local declarations such as `int x = 5`
- hybrid callable syntax such as `def<int> add(int x, int y) { ... }`

This repository is intentionally scaffolded as a course map.

## Pipeline Layout

- `include/adipy/`: public interfaces and shared compiler-facing types
- `src/frontend/`: source handling, lexing, AST, and parsing
- `src/sema/`: name resolution and type checking
- `src/ir/`: typed IR and lowering
- `src/interpreter/`: tier-0 execution
- `src/jit/`: machine-code generation and hotness-driven compilation
- `src/runtime/`: object model and GC support
- `src/driver/`: CLI commands
- `tests/`: subsystem-oriented tests

## Teacher Mode

The codebase currently contains declarations, signatures, and explanatory comments only.
Implementation bodies are intentionally left blank for guided development.
