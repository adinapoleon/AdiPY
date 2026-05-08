# Python-Inspired JIT in C++: v1 Plan

## Summary

Build a C++ JIT compiler/runtime for a **strongly typed, Python-inspired language** with **C-style braces/scopes**, targeting **x86-64 SysV on WSL2**. The implementation will be **interpreter first, then JIT**, so v1 behaves like a tiny HotSpot:

1. Parse source into AST
2. Type-check and resolve scopes/classes
3. Lower to a typed control-flow IR
4. Interpret IR while collecting hotness counts
5. JIT-compile hot functions/loops to x86-64
6. Use a **mark-and-sweep GC** for heap objects

This is not “Python syntax with annotations”; it is a **Pythonic language design with C-shaped surface syntax**.

## Key Changes / Implementation

### 1. Language definition to lock now

Surface language:

- Explicit types: `int`, `bool`, `void`, and class references
- Top-level functions and class methods
- Single inheritance with overriding
- `this` and `super`
- Braces define blocks; semicolons terminate statements
- No closures, nested functions, floats, strings, interfaces, generics, or multiple inheritance in v1

Statements in v1:

- variable declarations: `int x = 3;`
- assignment
- `if (...) { ... } else { ... }`
- `while (...) { ... }`
- `return ...;`
- `break;`
- `continue;`
- expression statements for calls

Recommended class shape:

```text
class Point : Object {
    int x;
    int y;

    void init(int a, int b) {
        this.x = a;
        this.y = b;
    }

    int sum() {
        return this.x + this.y;
    }
}
```

Default semantic rules:

- All locals must be declared before use
- No implicit type conversions except exact `bool` conditions
- Method override signatures must match exactly
- Field lookup is lexical by class hierarchy
- `super.method(...)` is statically resolved to the parent slot
- `null` exists only for class-reference types

### 2. Frontend architecture

Use a **handwritten lexer + handwritten parser**.

Lexer:
- single-pass scanner over source text
- track line/column for diagnostics
- emit tokens for identifiers, literals, keywords, punctuation, operators

Lock these token regexes:

```text
WHITESPACE   [ \t\r\n]+
LINE_COMMENT //[^\n]*
IDENT        [A-Za-z_][A-Za-z0-9_]*
INT_LIT      0|[1-9][0-9]*
LPAREN       \(
RPAREN       \)
LBRACE       \{
RBRACE       \}
COMMA        ,
SEMI         ;
COLON        :
DOT          \.
PLUS         \+
MINUS        -
STAR         \*
SLASH        /
PERCENT      %
ASSIGN       =
EQ           ==
NE           !=
LT           <
LE           <=
GT           >
GE           >=
AND          &&
OR           \|\|
NOT          !
```

Reserved keywords:

```text
class extends if else while return break continue
true false int bool void this super null
```

Parser:
- recursive descent for declarations/statements
- Pratt parser for expressions
- grammar shape:
  - program -> decl*
  - decl -> function_decl | class_decl | var_decl
  - class_decl -> `class` IDENT (`extends` IDENT)? `{` member* `}`
  - member -> field_decl | method_decl
  - function/method params are typed and named
  - expressions support precedence for call/member/unary/mul/add/compare/equality/and/or/assign

AST nodes to define:

- `Program`
- `ClassDecl`, `FieldDecl`, `MethodDecl`, `FunctionDecl`
- `BlockStmt`, `IfStmt`, `WhileStmt`, `ReturnStmt`, `BreakStmt`, `ContinueStmt`, `VarDeclStmt`, `ExprStmt`
- `AssignExpr`, `BinaryExpr`, `UnaryExpr`, `CallExpr`, `MemberExpr`, `VarExpr`, `ThisExpr`, `SuperExpr`, `IntLiteral`, `BoolLiteral`, `NullLiteral`

### 3. Semantic analysis and type system

Build these compiler passes after parsing:

1. **Name collection**
   - global function table
   - class table
   - parent-class links
2. **Inheritance validation**
   - no cycles
   - parent must exist
   - field/method layout derived top-down
3. **Scoped symbol resolution**
   - nested block scopes for locals
   - class scope for fields/methods
4. **Type checking**
   - expressions get fixed static types
   - assignments require exact compatibility
   - conditionals require `bool`
   - method calls checked against resolved signatures
5. **Override validation**
   - exact arity, parameter types, and return type match

Public compiler data structures to stabilize early:

- `Type` with variants: `Int`, `Bool`, `Void`, `ClassRef(class_id)`, `Null`
- `Symbol` for local/param/function/class/field/method
- `ClassInfo`:
  - class id
  - parent id
  - field layout with offsets
  - vtable slot map
- `FunctionSignature`
- `ResolvedExpr` / typed AST annotations

### 4. IR, interpreter, and JIT pipeline

Use a typed three-address IR inspired by your ILOC work, but extended for control flow and objects.

IR design:

- basic blocks with labels
- explicit terminators: `jump`, `branch`, `return`
- typed virtual registers
- instructions like:
  - `const_int`
  - `const_bool`
  - `move`
  - `add/sub/mul/div/mod`
  - `cmp_eq/ne/lt/le/gt/ge`
  - `load_local/store_local`
  - `load_field/store_field`
  - `new_object`
  - `load_vtable`
  - `call_static`
  - `call_virtual`
  - `guard_nonnull`
  - `jump`
  - `branch`
  - `return`

Lowering strategy:

- AST -> CFG IR per function/method
- `while` lowers to header/body/exit blocks
- `break` and `continue` use loop exit/header targets
- short-circuit `&&` and `||` lower through branches, not eager arithmetic

Execution tiers:

- **Tier 0 interpreter**
  - interpret IR directly
  - maintain per-function invocation counts and optional per-loop backedge counts
- **Tier 1 JIT**
  - compile hot functions first
  - optional loop hotness trigger later, but function-level hotness is the initial trigger

Hotness defaults:

- function hot threshold: 100 calls
- loop backedge threshold: record only in v1, don’t compile loops separately yet unless implementation stays simple

Backend:

- x86-64 machine-code emitter, direct bytes, no text assembly
- SysV calling convention
- simple prologue/epilogue
- patchable call targets for JITted functions
- baseline backend first:
  - no instruction scheduler in first codegen milestone
  - local register allocation first
  - graph scheduling can be a later optimization tier once correctness is stable

Register/codegen plan:

- start with a small fixed register set for integer/object refs
- keep values boxed only for heap objects; `int`/`bool` remain unboxed in registers/stack slots
- use stack slots for spills and interpreter/JIT frame metadata
- reserve conventions for:
  - frame pointer
  - stack pointer
  - runtime helper calls
  - GC safepoint metadata

### 5. Runtime and object model

Object layout:

- object header:
  - mark bit / GC flags
  - pointer to class metadata or vtable
- fields laid out in parent-first order
- one vtable per class
- overriding reuses parent slot
- new methods append slots

Runtime services:

- allocator for objects
- mark-and-sweep GC
- root discovery from:
  - interpreter frames
  - native JIT frames via explicit stack maps or conservative frame-root enumeration in v1
  - global/class metadata roots

Recommended GC staging:

1. start with stop-the-world mark-and-sweep
2. maintain exact roots for interpreter frames
3. for JIT frames, emit enough metadata to identify live object-reference stack slots and callee-saved registers at safepoints
4. allow GC only at safe helper-call boundaries in v1

Class/runtime metadata to expose:

- `RuntimeClass`
- `RuntimeObject`
- `VTable`
- `CompiledFunction`
- `InterpreterFrame`
- `JitFrameMap`

## Test Plan

### Frontend tests

- lexer tokenization for keywords, identifiers, comments, operators, braces
- parser precedence tests for arithmetic, comparisons, member access, and calls
- parse failures with correct source locations
- scope tests for shadowing and undeclared names
- type-check failures for mismatched assignments, bad return types, wrong method calls
- inheritance errors:
  - unknown parent
  - cyclic inheritance
  - bad override signature

### Semantic/control-flow tests

- `if/else` branch correctness
- `while` with `break` and `continue`
- nested block scope correctness
- short-circuit behavior for `&&` and `||`
- `this` field access and `super` method calls

### Runtime/object tests

- object allocation and field initialization
- inherited field layout
- method overriding and virtual dispatch
- null-check failures on member access/calls
- GC preserves reachable objects and reclaims unreachable ones

### JIT tests

- interpreter and JIT produce identical results for the same function
- hot function transitions from interpreted to native execution
- compiled calls to compiled and interpreted callees both work
- branch-heavy functions and loops compile correctly
- virtual dispatch still resolves correctly in JITted code
- GC at runtime helper safepoints does not corrupt object references

## Assumptions and Defaults

- Implementation language is **C++**, not Python
- Target platform is **x86-64 SysV ABI under WSL2/Linux**
- Parser is **handwritten**, not generated by PLY/SLY/bison
- Source language is **Python-inspired but not Python syntax**
- v1 built-in types are only `int`, `bool`, `void`, and class references
- v1 callable model is only **top-level functions + methods**
- v1 OOP is **single inheritance with overrides**
- v1 execution model is **IR interpreter first, then hot-function JIT**
- v1 GC is **stop-the-world mark-and-sweep**
- v1 optimization goal is correctness-first baseline JIT; advanced scheduling from your compilers work comes after baseline native execution is stable
