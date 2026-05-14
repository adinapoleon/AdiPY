# Adipy Skeleton Rebuild Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Replace the current implementation with a pipeline-first repository skeleton that preserves `docs/`, teaches the compiler architecture explicitly, and contains only teacher-mode declarations and comments.

**Architecture:** The rebuilt tree mirrors the compiler pipeline directly: `frontend`, `sema`, `ir`, `interpreter`, `jit`, `runtime`, `driver`, and `support`. Milestone 1 gets detailed public interfaces for source text, diagnostics, tokens, AST, and parsing, while later phases receive named skeletons that make the end-state architecture visible without implementing logic.

**Tech Stack:** C++20, GNU Make, standard library only, in-repo skeletal tests, Markdown docs.

---

## File Map

- Preserve: `docs/`
- Create: `.gitignore`
- Create: `Makefile`
- Create: `README.md`
- Create: `include/adipy/core/source_location.h`
- Create: `include/adipy/core/source_text.h`
- Create: `include/adipy/core/diagnostic.h`
- Create: `include/adipy/frontend/token.h`
- Create: `include/adipy/frontend/lexer.h`
- Create: `include/adipy/frontend/ast.h`
- Create: `include/adipy/frontend/parser.h`
- Create: `include/adipy/sema/type.h`
- Create: `include/adipy/sema/analyzer.h`
- Create: `include/adipy/ir/module.h`
- Create: `include/adipy/ir/lowering.h`
- Create: `include/adipy/interpreter/session.h`
- Create: `include/adipy/jit/compiler.h`
- Create: `include/adipy/runtime/object.h`
- Create: `include/adipy/driver/command.h`
- Create: `src/frontend/lexer.cpp`
- Create: `src/frontend/parser.cpp`
- Create: `src/sema/analyzer.cpp`
- Create: `src/ir/lowering.cpp`
- Create: `src/interpreter/session.cpp`
- Create: `src/jit/compiler.cpp`
- Create: `src/runtime/object.cpp`
- Create: `src/driver/main.cpp`
- Create: `src/support/README.md`
- Create: `tests/test_main.cpp`
- Create: `tests/frontend/lexer_tests.cpp`
- Create: `tests/frontend/parser_tests.cpp`

### Task 1: Reset the Tree and Recreate the Directory Skeleton

**Files:**
- Delete: every top-level path except `docs/` and `.git/`
- Create: `include/adipy/core/`
- Create: `include/adipy/frontend/`
- Create: `include/adipy/sema/`
- Create: `include/adipy/ir/`
- Create: `include/adipy/interpreter/`
- Create: `include/adipy/jit/`
- Create: `include/adipy/runtime/`
- Create: `include/adipy/driver/`
- Create: `src/frontend/`
- Create: `src/sema/`
- Create: `src/ir/`
- Create: `src/interpreter/`
- Create: `src/jit/`
- Create: `src/runtime/`
- Create: `src/driver/`
- Create: `src/support/`
- Create: `tests/frontend/`

- [ ] **Step 1: Inspect the pre-reset tree**

Run:

```bash
find /home/adinapoleon/Python_JIT -maxdepth 2 -mindepth 1 \
  \( -path /home/adinapoleon/Python_JIT/.git -o -path /home/adinapoleon/Python_JIT/docs \) -prune -o -print
```

Expected: the command lists the current implementation files that are about to be replaced, while skipping the preserved `docs/` tree and Git metadata.

- [ ] **Step 2: Remove the old implementation**

Run:

```bash
find /home/adinapoleon/Python_JIT -mindepth 1 -maxdepth 1 \
  ! -name .git ! -name docs -exec rm -rf {} +
```

Expected: the working tree contains only `docs/` and `.git/`.

- [ ] **Step 3: Recreate the directory structure**

Run:

```bash
mkdir -p \
  /home/adinapoleon/Python_JIT/include/adipy/core \
  /home/adinapoleon/Python_JIT/include/adipy/frontend \
  /home/adinapoleon/Python_JIT/include/adipy/sema \
  /home/adinapoleon/Python_JIT/include/adipy/ir \
  /home/adinapoleon/Python_JIT/include/adipy/interpreter \
  /home/adinapoleon/Python_JIT/include/adipy/jit \
  /home/adinapoleon/Python_JIT/include/adipy/runtime \
  /home/adinapoleon/Python_JIT/include/adipy/driver \
  /home/adinapoleon/Python_JIT/src/frontend \
  /home/adinapoleon/Python_JIT/src/sema \
  /home/adinapoleon/Python_JIT/src/ir \
  /home/adinapoleon/Python_JIT/src/interpreter \
  /home/adinapoleon/Python_JIT/src/jit \
  /home/adinapoleon/Python_JIT/src/runtime \
  /home/adinapoleon/Python_JIT/src/driver \
  /home/adinapoleon/Python_JIT/src/support \
  /home/adinapoleon/Python_JIT/tests/frontend
```

Expected: the repository has the full pipeline layout, even though only the frontend will have detailed interfaces in this pass.

- [ ] **Step 4: Verify the directory-only skeleton**

Run:

```bash
find /home/adinapoleon/Python_JIT -maxdepth 3 -mindepth 1 \
  \( -path /home/adinapoleon/Python_JIT/.git -o -path /home/adinapoleon/Python_JIT/docs \) -prune -o -type d -print | sort
```

Expected: the output shows the new phase-oriented directory structure and no leftover files from the previous implementation.

- [ ] **Step 5: Commit**

```bash
git add .
git commit -m "refactor: reset repository layout for compiler skeleton"
```

### Task 2: Add Repository Metadata and a Non-Implementing Build Scaffold

**Files:**
- Create: `.gitignore`
- Create: `Makefile`
- Create: `README.md`

- [ ] **Step 1: Create `.gitignore`**

Create `.gitignore`:

```gitignore
build/
*.o
*.obj
*.exe
*.out
```

- [ ] **Step 2: Create the Makefile scaffold**

Create `Makefile`:

```makefile
# This Makefile is intentionally a scaffold.
# It names the build entry points now so later milestones can fill in real recipes.

CXX ?= g++
CXXFLAGS ?= -std=c++20 -Wall -Wextra -Wpedantic -g
CPPFLAGS ?= -Iinclude

BUILD_DIR := build

.PHONY: all test clean

all:
	@printf 'Build scaffold only: no compilation rules are defined yet.\n'

test:
	@printf 'Test scaffold only: no executable tests are defined yet.\n'

clean:
	@printf 'Clean scaffold only: no build artifacts are expected yet.\n'
```

- [ ] **Step 3: Create the top-level README**

Create `README.md`:

```markdown
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
```

- [ ] **Step 4: Run scaffold verification**

Run:

```bash
make
make test
```

Expected:

- `make` prints the scaffold-only build message and exits successfully
- `make test` prints the scaffold-only test message and exits successfully

- [ ] **Step 5: Commit**

```bash
git add .gitignore Makefile README.md
git commit -m "build: add repository metadata scaffolding"
```

### Task 3: Define Milestone 1 Public Interfaces

**Files:**
- Create: `include/adipy/core/source_location.h`
- Create: `include/adipy/core/source_text.h`
- Create: `include/adipy/core/diagnostic.h`
- Create: `include/adipy/frontend/token.h`
- Create: `include/adipy/frontend/lexer.h`
- Create: `include/adipy/frontend/ast.h`
- Create: `include/adipy/frontend/parser.h`

- [ ] **Step 1: Add source location and source text interfaces**

Create `include/adipy/core/source_location.h`:

```cpp
#pragma once

#include <cstddef>

namespace adipy::core {

// Identifies one source coordinate for diagnostics and AST attachment points.
// Later frontend and sema phases depend on this remaining lightweight and stable.
struct SourceLocation {
    std::size_t line = 1;
    std::size_t column = 1;
};

// Names a half-open span in the original source text.
// The parser, diagnostics, and future typed IR lowering will use ranges to anchor errors.
struct SourceRange {
    SourceLocation begin;
    SourceLocation end;
};

}  // namespace adipy::core
```

Create `include/adipy/core/source_text.h`:

```cpp
#pragma once

#include <string>
#include <string_view>

namespace adipy::core {

// Wraps the raw program text and any stable metadata the frontend wants to carry with it.
// Keeping this separate from tokens makes later incremental parsing and file-based drivers cleaner.
struct SourceText {
    std::string path;
    std::string contents;

    std::string_view view() const;
};

}  // namespace adipy::core
```

- [ ] **Step 2: Add diagnostic interfaces**

Create `include/adipy/core/diagnostic.h`:

```cpp
#pragma once

#include <string>
#include <vector>

#include "adipy/core/source_location.h"

namespace adipy::core {

// Distinguishes syntax, semantic, and runtime-facing messages without coupling callers to formatting.
enum class DiagnosticSeverity {
    Note,
    Warning,
    Error,
};

// Represents one user-facing compiler message.
// Every phase should eventually report through this shape so the driver can render a uniform stream.
struct Diagnostic {
    DiagnosticSeverity severity;
    SourceRange range;
    std::string message;
};

// Aggregates phase diagnostics while keeping ownership simple for early milestones.
struct DiagnosticSet {
    std::vector<Diagnostic> items;

    bool has_errors() const;
};

}  // namespace adipy::core
```

- [ ] **Step 3: Add token and lexer interfaces**

Create `include/adipy/frontend/token.h`:

```cpp
#pragma once

#include <string>
#include <string_view>

#include "adipy/core/source_location.h"

namespace adipy::frontend {

// Enumerates the lexical categories required by the line-sensitive, brace-delimited surface language.
// NEWLINE is explicit because semicolons are absent and statement boundaries matter to the parser.
enum class TokenKind {
    EndOfFile,
    Newline,
    Identifier,
    IntegerLiteral,
    BooleanLiteral,

    KwClass,
    KwExtends,
    KwDef,
    KwIf,
    KwElse,
    KwWhile,
    KwReturn,
    KwBreak,
    KwContinue,
    KwTrue,
    KwFalse,
    KwInt,
    KwBool,
    KwVoid,

    LParen,
    RParen,
    LBrace,
    RBrace,
    Less,
    Greater,
    Comma,
    Dot,
    Assign,
    Plus,
    Minus,
    Star,
    Slash,
    Percent,
    PlusEqual,
    MinusEqual,
    EqualEqual,
    BangEqual,
    LessEqual,
    GreaterEqual,
    AndAnd,
    OrOr,
    Bang,
};

// Carries one token and the source slice it came from.
// The parser and diagnostics rely on lexeme preservation for precise error reporting.
struct Token {
    TokenKind kind;
    std::string lexeme;
    adipy::core::SourceRange range;
};

std::string_view token_kind_name(TokenKind kind);

}  // namespace adipy::frontend
```

Create `include/adipy/frontend/lexer.h`:

```cpp
#pragma once

#include <vector>

#include "adipy/core/diagnostic.h"
#include "adipy/core/source_text.h"
#include "adipy/frontend/token.h"

namespace adipy::frontend {

// Bundles the token stream and any syntax-level complaints produced during scanning.
// This boundary makes the parser independent from file I/O and diagnostic rendering.
struct LexResult {
    std::vector<Token> tokens;
    adipy::core::DiagnosticSet diagnostics;

    bool ok() const;
};

// Scans source text into tokens while enforcing newline significance rules.
// Later parser work depends on this contract for statement termination.
LexResult lex(const adipy::core::SourceText& source);

}  // namespace adipy::frontend
```

- [ ] **Step 4: Add AST and parser interfaces**

Create `include/adipy/frontend/ast.h`:

```cpp
#pragma once

#include <memory>
#include <string>
#include <vector>

#include "adipy/core/source_location.h"

namespace adipy::frontend {

struct TypeSyntax;
struct Expr;
struct Stmt;
struct Decl;
struct BlockStmt;

using ExprPtr = std::unique_ptr<Expr>;
using StmtPtr = std::unique_ptr<Stmt>;
using DeclPtr = std::unique_ptr<Decl>;

// Base node type for all frontend syntax objects.
// Keeping source attachment here lets later phases report errors against any node uniformly.
struct Node {
    adipy::core::SourceRange range;
};

// Names a type exactly as the parser saw it.
// Semantic analysis will later reinterpret this spelling into a resolved type graph.
struct TypeSyntax : Node {
    std::string spelling;
};

struct Decl : Node {
    virtual ~Decl();
};

struct Stmt : Node {
    virtual ~Stmt();
};

struct Expr : Node {
    virtual ~Expr();
};

// The root syntax container for one translation unit.
// Future semantic passes begin from this node.
struct Program : Node {
    std::vector<DeclPtr> declarations;
    std::vector<StmtPtr> top_level_statements;
};

// A hybrid callable declaration such as `def<int> add(int x, int y) { ... }`.
struct FunctionDecl : Decl {
    TypeSyntax return_type;
    std::string name;
    std::vector<TypeSyntax> parameter_types;
    std::vector<std::string> parameter_names;
    std::unique_ptr<BlockStmt> body;
};

// A class declaration with optional single inheritance.
struct ClassDecl : Decl {
    std::string name;
    std::string base_name;
    std::vector<DeclPtr> members;
};

// A field declaration using the language's C-style declaration syntax.
struct FieldDecl : Decl {
    TypeSyntax field_type;
    std::string name;
};

// A local declaration statement.
struct VarDeclStmt : Stmt {
    TypeSyntax declared_type;
    std::string name;
    ExprPtr initializer;
};

// A brace-delimited statement suite.
struct BlockStmt : Stmt {
    std::vector<StmtPtr> statements;
};

// A conditional statement.
struct IfStmt : Stmt {
    ExprPtr condition;
    StmtPtr then_branch;
    StmtPtr else_branch;
};

// A while loop statement.
struct WhileStmt : Stmt {
    ExprPtr condition;
    StmtPtr body;
};

// A return statement, optionally carrying a value.
struct ReturnStmt : Stmt {
    ExprPtr value;
};

struct BreakStmt : Stmt {};

struct ContinueStmt : Stmt {};

// A statement that exists for the side effects of evaluating an expression.
struct ExprStmt : Stmt {
    ExprPtr expression;
};

// An assignment expression.
struct AssignExpr : Expr {
    ExprPtr target;
    ExprPtr value;
};

// A binary operator expression.
struct BinaryExpr : Expr {
    std::string op;
    ExprPtr left;
    ExprPtr right;
};

// A unary operator expression.
struct UnaryExpr : Expr {
    std::string op;
    ExprPtr operand;
};

// A call expression.
struct CallExpr : Expr {
    ExprPtr callee;
    std::vector<ExprPtr> arguments;
};

// A member access expression such as `point.x`.
struct MemberExpr : Expr {
    ExprPtr object;
    std::string member_name;
};

// A simple named reference.
struct NameExpr : Expr {
    std::string name;
};

// An integer literal expression.
struct IntegerLiteralExpr : Expr {
    std::string lexeme;
};

// A boolean literal expression.
struct BooleanLiteralExpr : Expr {
    bool value = false;
};

}  // namespace adipy::frontend
```

Create `include/adipy/frontend/parser.h`:

```cpp
#pragma once

#include "adipy/core/diagnostic.h"
#include "adipy/core/source_text.h"
#include "adipy/frontend/ast.h"

namespace adipy::frontend {

// Carries the parsed syntax tree alongside parse-time diagnostics.
// Later sema entry points should consume this shape directly.
struct ParseResult {
    Program program;
    adipy::core::DiagnosticSet diagnostics;

    bool ok() const;
};

// Parses one source unit according to the line-sensitive, brace-delimited grammar.
ParseResult parse(const adipy::core::SourceText& source);

}  // namespace adipy::frontend
```

- [ ] **Step 5: Verify the public interface set**

Run:

```bash
rg --files /home/adinapoleon/Python_JIT/include/adipy
```

Expected: the include tree contains the milestone 1 public frontend headers and the named slots for later phases.

- [ ] **Step 6: Commit**

```bash
git add include/adipy
git commit -m "feat: add frontend interface skeletons"
```

### Task 4: Add Source Skeletons for the Frontend and Reserved Later Phases

**Files:**
- Create: `include/adipy/sema/type.h`
- Create: `include/adipy/sema/analyzer.h`
- Create: `include/adipy/ir/module.h`
- Create: `include/adipy/ir/lowering.h`
- Create: `include/adipy/interpreter/session.h`
- Create: `include/adipy/jit/compiler.h`
- Create: `include/adipy/runtime/object.h`
- Create: `include/adipy/driver/command.h`
- Create: `src/frontend/lexer.cpp`
- Create: `src/frontend/parser.cpp`
- Create: `src/sema/analyzer.cpp`
- Create: `src/ir/lowering.cpp`
- Create: `src/interpreter/session.cpp`
- Create: `src/jit/compiler.cpp`
- Create: `src/runtime/object.cpp`
- Create: `src/driver/main.cpp`
- Create: `src/support/README.md`

- [ ] **Step 1: Add later-phase public skeleton headers**

Create `include/adipy/sema/type.h`:

```cpp
#pragma once

#include <string>

namespace adipy::sema {

// Represents the semantic type layer that will sit above frontend type syntax.
// This exists now so later passes do not have to invent a home for resolved types.
struct Type {
    std::string debug_name;
};

}  // namespace adipy::sema
```

Create `include/adipy/sema/analyzer.h`:

```cpp
#pragma once

#include "adipy/core/diagnostic.h"
#include "adipy/frontend/ast.h"

namespace adipy::sema {

// Names the future semantic analysis result boundary.
// Type checking, scope resolution, and inheritance validation will eventually report through this API.
struct AnalysisResult {
    adipy::core::DiagnosticSet diagnostics;

    bool ok() const;
};

AnalysisResult analyze(const adipy::frontend::Program& program);

}  // namespace adipy::sema
```

Create `include/adipy/ir/module.h`:

```cpp
#pragma once

#include <string>
#include <vector>

namespace adipy::ir {

// Represents a future typed IR container.
// Lowering, interpretation, and JIT compilation will all depend on this boundary.
struct Function {
    std::string name;
};

struct Module {
    std::vector<Function> functions;
};

}  // namespace adipy::ir
```

Create `include/adipy/ir/lowering.h`:

```cpp
#pragma once

#include "adipy/frontend/ast.h"
#include "adipy/ir/module.h"

namespace adipy::ir {

// Defines the AST-to-IR boundary without committing to instruction selection yet.
Module lower_program(const adipy::frontend::Program& program);

}  // namespace adipy::ir
```

Create `include/adipy/interpreter/session.h`:

```cpp
#pragma once

#include "adipy/ir/module.h"

namespace adipy::interpreter {

// Marks the future tier-0 execution surface for IR.
// Hotness tracking and debug stepping will eventually hang off this session object.
struct Session {};

void load_module(Session& session, const adipy::ir::Module& module);

}  // namespace adipy::interpreter
```

Create `include/adipy/jit/compiler.h`:

```cpp
#pragma once

#include "adipy/ir/module.h"

namespace adipy::jit {

// Reserves the ownership boundary for compiled code generation and native entry management.
struct Compiler {};

void enqueue_module(Compiler& compiler, const adipy::ir::Module& module);

}  // namespace adipy::jit
```

Create `include/adipy/runtime/object.h`:

```cpp
#pragma once

#include <cstddef>

namespace adipy::runtime {

// Names the future runtime object header and allocation boundary.
// The interpreter and JIT will eventually share this layout contract.
struct ObjectHeader {
    std::size_t size = 0;
};

}  // namespace adipy::runtime
```

Create `include/adipy/driver/command.h`:

```cpp
#pragma once

#include <string>
#include <vector>

namespace adipy::driver {

// Enumerates the user-facing driver modes the CLI will eventually support.
enum class CommandKind {
    Lex,
    Parse,
    Run,
    DumpIr,
    JitStats,
};

// Captures one parsed command invocation before execution begins.
struct CommandLine {
    CommandKind kind;
    std::string path;
    std::vector<std::string> arguments;
};

int run_command(const CommandLine& command_line);

}  // namespace adipy::driver
```

- [ ] **Step 2: Add frontend implementation skeleton files**

Create `src/frontend/lexer.cpp`:

```cpp
#include "adipy/frontend/lexer.h"

namespace adipy::frontend {

// This translation unit will eventually hold the newline-sensitive scanner.
// Definitions are intentionally omitted so the student can build the lexer mechanics manually.

}  // namespace adipy::frontend
```

Create `src/frontend/parser.cpp`:

```cpp
#include "adipy/frontend/parser.h"

namespace adipy::frontend {

// This translation unit will eventually own declaration parsing, statement parsing,
// and expression precedence handling for the hybrid source language.
// Definitions are intentionally omitted in teacher mode.

}  // namespace adipy::frontend
```

- [ ] **Step 3: Add later-phase implementation skeleton files**

Create `src/sema/analyzer.cpp`:

```cpp
#include "adipy/sema/analyzer.h"

namespace adipy::sema {

// Semantic analysis definitions belong here once the frontend syntax tree is stable.
// The file exists now to make the phase boundary visible from the start.

}  // namespace adipy::sema
```

Create `src/ir/lowering.cpp`:

```cpp
#include "adipy/ir/lowering.h"

namespace adipy::ir {

// AST-to-IR lowering definitions belong here once semantic analysis can provide resolved input.
// Keeping the file now prevents IR from becoming an afterthought in the repository layout.

}  // namespace adipy::ir
```

Create `src/interpreter/session.cpp`:

```cpp
#include "adipy/interpreter/session.h"

namespace adipy::interpreter {

// Tier-0 execution support will eventually live here.
// The empty scaffold keeps interpretation as a first-class planned milestone.

}  // namespace adipy::interpreter
```

Create `src/jit/compiler.cpp`:

```cpp
#include "adipy/jit/compiler.h"

namespace adipy::jit {

// Native code generation and code cache management will eventually live here.
// The scaffold makes the backend boundary explicit before any machine code exists.

}  // namespace adipy::jit
```

Create `src/runtime/object.cpp`:

```cpp
#include "adipy/runtime/object.h"

namespace adipy::runtime {

// Runtime object support, allocation, and GC integration will eventually live here.
// The file exists now so runtime concerns stay architecturally visible.

}  // namespace adipy::runtime
```

Create `src/driver/main.cpp`:

```cpp
#include "adipy/driver/command.h"

namespace adipy::driver {

// Driver command execution will eventually bridge CLI input to frontend, sema, IR, and runtime phases.
// Definitions are intentionally omitted until the student chooses how command dispatch should work.

}  // namespace adipy::driver
```

Create `src/support/README.md`:

```markdown
# Support

This directory is reserved for compiler-internal helpers that do not belong to one phase.

Examples of future residents:

- text formatting helpers
- owning and non-owning utility wrappers
- debug rendering helpers
- shared assertions or tracing support
```

- [ ] **Step 4: Verify the phase skeleton**

Run:

```bash
find /home/adinapoleon/Python_JIT/include /home/adinapoleon/Python_JIT/src -type f | sort
```

Expected: every phase named in the design has at least one tracked file, and milestone 1 frontend files are visibly richer than later-phase placeholders.

- [ ] **Step 5: Commit**

```bash
git add include/adipy src
git commit -m "feat: add pipeline skeleton files"
```

### Task 5: Add Frontend Test Skeletons and Final Structure Verification

**Files:**
- Create: `tests/test_main.cpp`
- Create: `tests/frontend/lexer_tests.cpp`
- Create: `tests/frontend/parser_tests.cpp`

- [ ] **Step 1: Create the test runner shell**

Create `tests/test_main.cpp`:

```cpp
#include <string_view>

namespace adipy::tests {

// Declares the frontend test groups without implementing them.
// This keeps the eventual test taxonomy visible without solving the logic now.
void run_lexer_tests();
void run_parser_tests();

}  // namespace adipy::tests

// The real test entrypoint is intentionally deferred.
// This file currently documents the intended harness surface only.
```

- [ ] **Step 2: Create the lexer test skeleton**

Create `tests/frontend/lexer_tests.cpp`:

```cpp
namespace adipy::tests {

// Lexer tests will eventually cover:
// - newline token emission rules
// - brace and paren punctuation
// - hybrid callable syntax tokens such as def<int>
// - keyword versus identifier classification
// - line and column tracking in diagnostics
void run_lexer_tests();

}  // namespace adipy::tests
```

- [ ] **Step 3: Create the parser test skeleton**

Create `tests/frontend/parser_tests.cpp`:

```cpp
namespace adipy::tests {

// Parser tests will eventually cover:
// - newline-terminated variable declarations
// - brace-delimited blocks without semicolons
// - def<int> function declarations
// - class declarations with extends
// - top-level statements plus explicit main entry conventions
// - expression precedence and assignment parsing
void run_parser_tests();

}  // namespace adipy::tests
```

- [ ] **Step 4: Verify the rebuilt tree**

Run:

```bash
find /home/adinapoleon/Python_JIT -maxdepth 4 -mindepth 1 \
  \( -path /home/adinapoleon/Python_JIT/.git -o -path /home/adinapoleon/Python_JIT/docs \) -prune -o -print | sort
```

Expected: the old implementation is gone, the new compiler-phase layout is present, and only the preserved `docs/` tree plus the new skeleton files remain.

- [ ] **Step 5: Verify the scaffold commands still succeed**

Run:

```bash
make
make test
```

Expected:

- `make` succeeds with the build-scaffold message
- `make test` succeeds with the test-scaffold message

- [ ] **Step 6: Commit**

```bash
git add tests
git commit -m "test: add frontend skeleton tests"
```
