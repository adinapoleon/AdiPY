# Adipy Skeleton Rebuild Design

## Goal

Rebuild the repository from scratch while preserving the `docs/` tree, using a pipeline-first compiler layout and a strict teacher-mode workflow. The rebuilt project should expose only commented skeletons for milestone 1, while reserving clear structural space for later semantic analysis, IR, interpreter, JIT, and runtime work.

## Repository Reset Intent

The current repository contains partial lexer and parser implementation work, but the reset will treat that implementation as disposable. The preserved source of truth is the design intent captured in `docs/`, not the current code.

After this design is approved:

- everything outside `docs/` may be deleted
- the repository will be recreated with a new top-level structure
- new source files will contain declarations, signatures, and explanatory comments only
- implementation logic will be left blank for the student to write

## Language Surface for Milestone 1

Milestone 1 defines the lexical and syntactic surface of the language, but not semantic analysis or execution.

### Block and statement structure

- Blocks are defined by `{` and `}`.
- Grouping expressions and call syntax use `(` and `)`.
- Semicolons are not part of the language.
- Statements are terminated by newline, unless the lexer determines the line is inside an open grouping construct or the expression is clearly continued.

This makes newline handling part of the formal frontend contract rather than just whitespace skipping.

### Declaration syntax

Local and field declarations follow a C-style declaration head:

```text
int x = 5
bool ready = true
```

This keeps the type in a position familiar from C-family languages and makes declaration parsing distinct from ordinary assignment parsing.

### Function syntax

Functions use a hybrid callable declaration head:

```text
def<int> add(int x, int y) {
    return x + y
}
```

In this language, `def<ReturnType>` is not generic syntax. The angle brackets are reserved specifically as return type syntax for callable declarations.

### Class syntax

Classes use explicit inheritance keywords:

```text
class Point extends Object {
    int x
    int y
}
```

Using `extends` keeps inheritance visible in the token stream and simplifies both parsing and semantic analysis.

### Top-level model

Source files may contain top-level statements, declarations, or both, but execution still uses an explicit main-style entrypoint. This keeps the runtime model closer to C++ or Java than to Python script execution, while still allowing flexible source layout.

## Frontend Design Consequences

The lexer must do more than classify lexemes. It must also:

- track line and column information for diagnostics
- emit `NEWLINE` tokens only when they are syntactically meaningful
- suppress newline significance inside open grouping contexts
- provide enough token information for a recursive-descent parser to distinguish declaration starts from statement starts

The parser design for milestone 1 should therefore assume:

- recursive descent for declarations and statements
- precedence-based or Pratt-style parsing for expressions
- explicit handling for newline-terminated statements
- syntax diagnostics that reference source locations and statement boundaries

## Repository Structure

The repository should teach the compiler pipeline directly through its layout.

### Public interface layer

- `include/adipy/`

This directory holds stable public headers and shared compiler-facing data types. It is the boundary other phases depend on.

### Frontend

- `src/frontend/`

This directory owns source text processing, tokenization, AST structure, and parser-facing diagnostics. Milestone 1 lives primarily here.

### Semantic analysis

- `src/sema/`

This directory is reserved for name collection, scope resolution, inheritance validation, and type checking.

### Intermediate representation

- `src/ir/`

This directory is reserved for typed IR definitions, CFG units, and lowering infrastructure.

### Interpreter

- `src/interpreter/`

This directory is reserved for tier-0 execution of IR and hotness instrumentation.

### JIT backend

- `src/jit/`

This directory is reserved for machine code emission, compiled function management, and backend entrypoints.

### Runtime

- `src/runtime/`

This directory is reserved for object layout, allocation, GC, vtables, and helper services shared by the interpreter and JIT.

### Driver

- `src/driver/`

This directory holds CLI entrypoints and command dispatch such as lexing, parsing, running, and future debug dumps.

### Shared support utilities

- `src/support/`

This directory holds compiler-internal helpers that are not owned by one specific phase.

### Tests

- `tests/`

Tests should be organized by subsystem so that frontend, semantic analysis, IR, runtime, and JIT behavior can evolve independently.

## Milestone Strategy

The rebuild should make the whole compiler visible, but only milestone 1 gets detailed interfaces now.

### Milestone 1

- source management
- tokens and diagnostics
- lexer interfaces
- AST interfaces
- parser interfaces
- CLI or driver skeleton for frontend commands
- frontend-focused test skeletons

### Milestone 2

- symbols
- scopes
- class metadata scaffolding
- type system interfaces
- semantic analysis entrypoints

### Milestone 3

- typed IR
- control-flow graph structures
- lowering interfaces from typed AST to IR

### Milestone 4

- IR interpreter
- hotness counters
- execution context abstractions

### Milestone 5

- JIT manager
- backend interfaces
- machine code buffer abstractions

### Milestone 6

- runtime object model
- allocation interfaces
- GC scaffolding
- JIT/runtime integration surfaces

## Teacher-Mode File Policy

Every new non-doc file created during the reset must follow the same discipline:

- include only declarations, signatures, enums, structs, classes, and comments
- omit implementation logic
- explain what the file is for
- explain why the abstraction exists
- explain which later phase depends on it

Comments should not narrate syntax mechanically. They should explain architectural purpose and compiler-phase relationships.

## Skeleton Philosophy

The rebuilt repository should behave like a course map rather than a hidden implementation. A reader should be able to open the tree and understand:

- what the compiler phases are
- what each phase is responsible for
- which interfaces connect phases together
- what work is intentionally deferred

This means the reset should prefer explicit placeholders with strong comments over partial logic that obscures the architecture.

## Initial Rebuild Scope

Once implementation begins, the first destructive rebuild pass should:

1. preserve `docs/`
2. remove all other project files and directories that are part of the old implementation
3. recreate the top-level source and test layout
4. add milestone 1 skeleton headers and source files
5. add placeholder directories or minimal marker files for later compiler stages
6. keep all logic blank for the student to fill in later

## Open Boundaries to Revisit Later

The following design choices are intentionally deferred until later milestones:

- exact type system representation beyond early builtin types
- detailed AST ownership and memory strategy
- typed IR instruction set
- interpreter frame layout
- object layout and vtable layout
- JIT calling convention details
- GC root map representation

These are major compiler design questions, but they do not need to be frozen before the frontend skeleton exists.
