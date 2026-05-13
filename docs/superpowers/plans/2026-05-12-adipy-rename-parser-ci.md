# Adipy Rename, CI, and Parser Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Rename the project from `pyjit` to `adipy`, add CI for build and tests, and implement the next compiler milestone after the lexer: an AST-backed handwritten parser with tests.

**Architecture:** Keep the current small C++ layout, but rename the public include tree and namespace to `adipy`. Add parser-facing public headers beside the lexer API, implement recursive-descent declaration and statement parsing plus Pratt-style expression parsing in `src/frontend`, and expose a CLI syntax-check command for manual use.

**Tech Stack:** C++20, GNU Make, standard library only, GitHub Actions, in-repo test harness.

---

## File Map

- Modify: `Makefile`
- Modify: `README.md`
- Modify: `.gitignore` only if needed for new artifacts
- Rename: `include/pyjit/` to `include/adipy/`
- Modify: `include/adipy/source_location.h`
- Modify: `include/adipy/token.h`
- Modify: `include/adipy/lexer.h`
- Create: `include/adipy/ast.h`
- Create: `include/adipy/parser.h`
- Modify: `src/frontend/lexer.cpp`
- Create: `src/frontend/parser.cpp`
- Modify: `src/cli/main.cpp`
- Modify: `tests/test_main.cpp`
- Modify: `tests/lexer_tests.cpp`
- Create: `tests/parser_tests.cpp`
- Create: `.github/workflows/ci.yml`
- Modify: `docs/superpowers/specs/2026-05-10-project-scaffold-design.md`
- Modify: `docs/superpowers/plans/2026-05-10-project-scaffold-lexer.md`

### Task 1: Rename Public Surface to `adipy`

**Files:**
- Rename: `include/pyjit/` to `include/adipy/`
- Modify: `Makefile`
- Modify: `README.md`
- Modify: `src/cli/main.cpp`
- Modify: `src/frontend/lexer.cpp`
- Modify: `tests/test_main.cpp`
- Modify: `tests/lexer_tests.cpp`
- Modify: docs under `docs/superpowers/`

- [ ] **Step 1: Change references in tests first**

Update test includes and expected CLI names from `pyjit` to `adipy`, and add the new test source file name to the build list so the build fails until implementation catches up.

- [ ] **Step 2: Run verification to watch the rename fail**

Run: `make test`
Expected: FAIL because production headers, namespace names, or binary names still use `pyjit`.

- [ ] **Step 3: Apply the rename across production code**

Rename the public include directory, switch `namespace pyjit` to `namespace adipy`, update include paths, and rename Makefile binary targets from `pyjit`/`pyjit_tests` to `adipy`/`adipy_tests`.

- [ ] **Step 4: Update docs and CLI strings**

Change the README and design/plan docs so the project name and sample commands consistently use `adipy`.

- [ ] **Step 5: Rebuild**

Run: `make test`
Expected: build progresses again, with parser-related failures still acceptable until the new parser files exist.

### Task 2: Add Parser Tests First

**Files:**
- Modify: `Makefile`
- Modify: `tests/test_main.cpp`
- Create: `tests/parser_tests.cpp`

- [ ] **Step 1: Add parser test runner hook**

Declare `run_parser_tests()` from `tests/test_main.cpp` and call it after lexer tests.

- [ ] **Step 2: Write failing parser tests**

Cover:
- parsing a top-level function with typed parameters and a return statement
- parsing a class with `extends`, fields, and methods
- expression precedence for assignment, logical, comparison, additive, multiplicative, call, and member access
- statement parsing for `if`/`else`, `while`, `break`, `continue`, and block-scoped variable declarations
- syntax diagnostics with line/column on malformed input

- [ ] **Step 3: Add parser test source to the Makefile**

Include `tests/parser_tests.cpp` in `TEST_SRCS` and add the future parser implementation file to frontend sources.

- [ ] **Step 4: Run parser tests to verify RED**

Run: `make test`
Expected: FAIL because `adipy/parser.h`, `src/frontend/parser.cpp`, or parser symbols do not exist yet.

### Task 3: Define AST and Parser Interfaces

**Files:**
- Create: `include/adipy/ast.h`
- Create: `include/adipy/parser.h`

- [ ] **Step 1: Add the AST model**

Define a small AST that matches the design docs:
- program, class, field, method, function declarations
- statement variants for block, `if`, `while`, `return`, `break`, `continue`, variable declaration, expression statement
- expression variants for assignment, binary, unary, call, member, variable, `this`, `super`, integer/bool/null literals

- [ ] **Step 2: Add parser diagnostics and public API**

Expose a `ParseResult` with `Program`, diagnostics, and an `ok()` helper. Provide a `parse(std::string_view source)` entry point that lexes then parses.

- [ ] **Step 3: Run parser tests again**

Run: `make test`
Expected: FAIL at link time or behavior assertions because the parser implementation still does not exist.

### Task 4: Implement the Parser

**Files:**
- Create: `src/frontend/parser.cpp`

- [ ] **Step 1: Implement declaration parsing**

Support top-level class declarations, function declarations, and variable declarations. Accept `extends` for inheritance spelling.

- [ ] **Step 2: Implement statement parsing**

Support blocks, `if`/`else`, `while`, `return`, `break`, `continue`, variable declarations, and expression statements.

- [ ] **Step 3: Implement Pratt-style expression parsing**

Support:
- primary expressions for literals, identifiers, `this`, `super`, and parenthesized expressions
- postfix/member/call parsing
- unary `!`/`-`
- multiplicative, additive, comparison, equality, logical `&&`/`||`
- right-associative assignment

- [ ] **Step 4: Implement parser diagnostics**

Report concise syntax errors with source locations and stop after the first parser error, matching current lexer behavior.

- [ ] **Step 5: Run tests to verify GREEN**

Run: `make test`
Expected: PASS.

### Task 5: Add CLI Parse Command and CI

**Files:**
- Modify: `src/cli/main.cpp`
- Create: `.github/workflows/ci.yml`

- [ ] **Step 1: Add `adipy parse <path>`**

Read the source file, invoke the parser, print the first diagnostic on failure, and return success on syntactically valid input.

- [ ] **Step 2: Add GitHub Actions workflow**

Create a workflow triggered on `push` and `pull_request` that checks out the repo, installs a C++ compiler if needed, runs `make`, and then runs `make test`.

- [ ] **Step 3: Verify full build and test flow**

Run:
- `make clean`
- `make`
- `make test`

Expected: all commands succeed.
