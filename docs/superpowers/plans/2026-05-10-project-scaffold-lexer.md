# Project Scaffold and Lexer Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Build the first working C++ slice: Makefile, CLI entry point, token model, handwritten lexer, and lightweight lexer tests.

**Architecture:** Keep frontend code in `include/adipy` and `src/frontend`, with the CLI in `src/cli`. The lexer is a deterministic scanner over `std::string_view`, returning either tokens or one concise diagnostic.

**Tech Stack:** C++20, GNU Make, standard library only, in-repo test harness.

---

## File Map

- Create: `.gitignore` if missing; ensure it ignores `build/` and `docs/superpowers/specs/*.md`
- Create: `Makefile` for CLI and test builds
- Create: `include/adipy/source_location.h` for source coordinates
- Create: `include/adipy/token.h` for token kinds, token values, and display helpers
- Create: `include/adipy/lexer.h` for lexer API and diagnostic types
- Create: `src/frontend/lexer.cpp` for scanner implementation
- Create: `src/cli/main.cpp` for `adipy lex <path>`
- Create: `tests/test_main.cpp` for the test runner entry point
- Create: `tests/lexer_tests.cpp` for lexer behavior tests

## Task 1: Build Skeleton

**Files:**
- Create: `Makefile`
- Modify: `.gitignore`

- [ ] **Step 1: Add or verify `.gitignore`**

Ensure `.gitignore` contains exactly these project-specific entries:

```gitignore
build/
docs/superpowers/specs/*.md
```

- [ ] **Step 2: Create the Makefile**

Create `Makefile`:

```makefile
CXX ?= g++
CXXFLAGS ?= -std=c++20 -Wall -Wextra -Wpedantic -g
CPPFLAGS ?= -Iinclude

BUILD_DIR := build
CLI_BIN := $(BUILD_DIR)/adipy
TEST_BIN := $(BUILD_DIR)/adipy_tests

FRONTEND_SRCS := src/frontend/lexer.cpp
CLI_SRCS := src/cli/main.cpp
TEST_SRCS := tests/test_main.cpp tests/lexer_tests.cpp

.PHONY: all test clean

all: $(CLI_BIN)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(CLI_BIN): $(CLI_SRCS) $(FRONTEND_SRCS) | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@

$(TEST_BIN): $(TEST_SRCS) $(FRONTEND_SRCS) | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@

test: $(TEST_BIN)
	$(TEST_BIN)

clean:
	rm -rf $(BUILD_DIR)
```

- [ ] **Step 3: Run the empty build**

Run: `make`

Expected: FAIL because source files do not exist yet. This confirms the Makefile target is wired to the intended file paths.

- [ ] **Step 4: Commit**

```bash
git add .gitignore Makefile
git commit -m "build: add makefile scaffold"
```

## Task 2: Public Token and Lexer Interfaces

**Files:**
- Create: `include/adipy/source_location.h`
- Create: `include/adipy/token.h`
- Create: `include/adipy/lexer.h`

- [ ] **Step 1: Create source location type**

Create `include/adipy/source_location.h`:

```cpp
#pragma once

#include <cstddef>

namespace adipy {

struct SourceLocation {
    std::size_t line = 1;
    std::size_t column = 1;
};

}  // namespace adipy
```

- [ ] **Step 2: Create token kinds and token type**

Create `include/adipy/token.h`:

```cpp
#pragma once

#include <string>
#include <string_view>

#include "adipy/source_location.h"

namespace adipy {

enum class TokenKind {
    EndOfFile,
    Identifier,
    IntLiteral,

    KwClass,
    KwExtends,
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
    KwThis,
    KwSuper,
    KwNull,

    LParen,
    RParen,
    LBrace,
    RBrace,
    Comma,
    Semi,
    Colon,
    Dot,

    Plus,
    Minus,
    Star,
    Slash,
    Percent,
    Assign,
    Eq,
    Ne,
    Lt,
    Le,
    Gt,
    Ge,
    And,
    Or,
    Not,
};

struct Token {
    TokenKind kind;
    std::string lexeme;
    SourceLocation location;
};

std::string_view token_kind_name(TokenKind kind);

}  // namespace adipy
```

- [ ] **Step 3: Create lexer API**

Create `include/adipy/lexer.h`:

```cpp
#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "adipy/source_location.h"
#include "adipy/token.h"

namespace adipy {

struct LexerDiagnostic {
    SourceLocation location;
    std::string message;
};

struct LexResult {
    std::vector<Token> tokens;
    std::vector<LexerDiagnostic> diagnostics;

    bool ok() const {
        return diagnostics.empty();
    }
};

LexResult lex(std::string_view source);

}  // namespace adipy
```

- [ ] **Step 4: Run build**

Run: `make`

Expected: FAIL because `src/frontend/lexer.cpp` and `src/cli/main.cpp` do not exist yet.

- [ ] **Step 5: Commit**

```bash
git add include/adipy/source_location.h include/adipy/token.h include/adipy/lexer.h
git commit -m "feat: define lexer public interfaces"
```

## Task 3: Lexer Tests First

**Files:**
- Create: `tests/test_main.cpp`
- Create: `tests/lexer_tests.cpp`

- [ ] **Step 1: Create test runner declarations**

Create `tests/test_main.cpp`:

```cpp
#include <exception>
#include <iostream>

void run_lexer_tests();

int main() {
    try {
        run_lexer_tests();
    } catch (const std::exception& error) {
        std::cerr << "test failure: " << error.what() << '\n';
        return 1;
    }

    std::cout << "all tests passed\n";
    return 0;
}
```

- [ ] **Step 2: Create lexer tests**

Create `tests/lexer_tests.cpp`:

```cpp
#include "adipy/lexer.h"

#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

using adipy::TokenKind;

void require(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void require_ok(const adipy::LexResult& result) {
    if (!result.ok()) {
        std::ostringstream out;
        out << result.diagnostics.front().location.line << ':'
            << result.diagnostics.front().location.column << ' '
            << result.diagnostics.front().message;
        throw std::runtime_error(out.str());
    }
}

std::vector<TokenKind> kinds_without_eof(const adipy::LexResult& result) {
    std::vector<TokenKind> kinds;
    for (const auto& token : result.tokens) {
        if (token.kind != TokenKind::EndOfFile) {
            kinds.push_back(token.kind);
        }
    }
    return kinds;
}

void expect_kinds(const std::string& source, const std::vector<TokenKind>& expected) {
    auto result = adipy::lex(source);
    require_ok(result);
    require(kinds_without_eof(result) == expected, "token kind sequence mismatch");
}

void test_keywords_and_identifiers() {
    auto result = adipy::lex("class classic extends iffy if else while return break continue true false int bool void this super null name_1");
    require_ok(result);

    const std::vector<TokenKind> expected = {
        TokenKind::KwClass,
        TokenKind::Identifier,
        TokenKind::KwExtends,
        TokenKind::Identifier,
        TokenKind::KwIf,
        TokenKind::KwElse,
        TokenKind::KwWhile,
        TokenKind::KwReturn,
        TokenKind::KwBreak,
        TokenKind::KwContinue,
        TokenKind::KwTrue,
        TokenKind::KwFalse,
        TokenKind::KwInt,
        TokenKind::KwBool,
        TokenKind::KwVoid,
        TokenKind::KwThis,
        TokenKind::KwSuper,
        TokenKind::KwNull,
        TokenKind::Identifier,
    };

    require(kinds_without_eof(result) == expected, "keyword tokenization mismatch");
    require(result.tokens[1].lexeme == "classic", "identifier lexeme mismatch");
}

void test_comments_and_whitespace() {
    expect_kinds("int x // ignored\nbool y", {
        TokenKind::KwInt,
        TokenKind::Identifier,
        TokenKind::KwBool,
        TokenKind::Identifier,
    });
}

void test_single_character_tokens() {
    expect_kinds("(){} ,;:.+-*/%=<>!", {
        TokenKind::LParen,
        TokenKind::RParen,
        TokenKind::LBrace,
        TokenKind::RBrace,
        TokenKind::Comma,
        TokenKind::Semi,
        TokenKind::Colon,
        TokenKind::Dot,
        TokenKind::Plus,
        TokenKind::Minus,
        TokenKind::Star,
        TokenKind::Slash,
        TokenKind::Percent,
        TokenKind::Assign,
        TokenKind::Lt,
        TokenKind::Gt,
        TokenKind::Not,
    });
}

void test_two_character_tokens() {
    expect_kinds("== != <= >= && ||", {
        TokenKind::Eq,
        TokenKind::Ne,
        TokenKind::Le,
        TokenKind::Ge,
        TokenKind::And,
        TokenKind::Or,
    });
}

void test_locations() {
    auto result = adipy::lex("int\n  value");
    require_ok(result);
    require(result.tokens[0].location.line == 1, "first token line mismatch");
    require(result.tokens[0].location.column == 1, "first token column mismatch");
    require(result.tokens[1].location.line == 2, "second token line mismatch");
    require(result.tokens[1].location.column == 3, "second token column mismatch");
}

void test_invalid_character() {
    auto result = adipy::lex("@");
    require(!result.ok(), "invalid character should fail");
    require(result.diagnostics.front().location.line == 1, "invalid character line mismatch");
    require(result.diagnostics.front().location.column == 1, "invalid character column mismatch");
}

void test_standalone_ampersand_and_pipe() {
    auto amp = adipy::lex("&");
    require(!amp.ok(), "standalone ampersand should fail");
    require(amp.diagnostics.front().message == "unexpected character '&'; did you mean '&&'?", "ampersand diagnostic mismatch");

    auto pipe = adipy::lex("|");
    require(!pipe.ok(), "standalone pipe should fail");
    require(pipe.diagnostics.front().message == "unexpected character '|'; did you mean '||'?", "pipe diagnostic mismatch");
}

void test_integer_literals() {
    auto result = adipy::lex("0 12 345");
    require_ok(result);
    require(result.tokens[0].lexeme == "0", "zero literal mismatch");
    require(result.tokens[1].lexeme == "12", "integer literal mismatch");
    require(result.tokens[2].lexeme == "345", "integer literal mismatch");

    auto leading_zero = adipy::lex("012");
    require(!leading_zero.ok(), "leading zero integer should fail");
    require(leading_zero.diagnostics.front().message == "invalid integer literal with leading zero", "leading zero diagnostic mismatch");
}

}  // namespace

void run_lexer_tests() {
    test_keywords_and_identifiers();
    test_comments_and_whitespace();
    test_single_character_tokens();
    test_two_character_tokens();
    test_locations();
    test_invalid_character();
    test_standalone_ampersand_and_pipe();
    test_integer_literals();
}
```

- [ ] **Step 3: Run tests to verify failure**

Run: `make test`

Expected: FAIL because `src/frontend/lexer.cpp` is not implemented yet.

- [ ] **Step 4: Commit**

```bash
git add tests/test_main.cpp tests/lexer_tests.cpp
git commit -m "test: add lexer behavior tests"
```

## Task 4: Lexer Implementation

**Files:**
- Create: `src/frontend/lexer.cpp`

- [ ] **Step 1: Implement token display and scanner**

Create `src/frontend/lexer.cpp`:

```cpp
#include "adipy/lexer.h"

#include <cctype>
#include <string_view>
#include <unordered_map>

namespace adipy {
namespace {

bool is_identifier_start(char ch) {
    return std::isalpha(static_cast<unsigned char>(ch)) || ch == '_';
}

bool is_identifier_continue(char ch) {
    return std::isalnum(static_cast<unsigned char>(ch)) || ch == '_';
}

const std::unordered_map<std::string_view, TokenKind>& keywords() {
    static const std::unordered_map<std::string_view, TokenKind> table = {
        {"class", TokenKind::KwClass},
        {"extends", TokenKind::KwExtends},
        {"if", TokenKind::KwIf},
        {"else", TokenKind::KwElse},
        {"while", TokenKind::KwWhile},
        {"return", TokenKind::KwReturn},
        {"break", TokenKind::KwBreak},
        {"continue", TokenKind::KwContinue},
        {"true", TokenKind::KwTrue},
        {"false", TokenKind::KwFalse},
        {"int", TokenKind::KwInt},
        {"bool", TokenKind::KwBool},
        {"void", TokenKind::KwVoid},
        {"this", TokenKind::KwThis},
        {"super", TokenKind::KwSuper},
        {"null", TokenKind::KwNull},
    };
    return table;
}

class Lexer {
public:
    explicit Lexer(std::string_view source) : source_(source) {}

    LexResult run() {
        while (!is_at_end() && diagnostics_.empty()) {
            skip_trivia();
            if (!is_at_end()) {
                scan_token();
            }
        }

        tokens_.push_back(Token{TokenKind::EndOfFile, "", location_});
        return LexResult{tokens_, diagnostics_};
    }

private:
    bool is_at_end() const {
        return index_ >= source_.size();
    }

    char peek() const {
        return is_at_end() ? '\0' : source_[index_];
    }

    char peek_next() const {
        return index_ + 1 >= source_.size() ? '\0' : source_[index_ + 1];
    }

    char advance() {
        const char ch = source_[index_++];
        if (ch == '\n') {
            ++location_.line;
            location_.column = 1;
        } else {
            ++location_.column;
        }
        return ch;
    }

    bool match(char expected) {
        if (is_at_end() || source_[index_] != expected) {
            return false;
        }
        advance();
        return true;
    }

    void add_token(TokenKind kind, SourceLocation start, std::size_t start_index) {
        tokens_.push_back(Token{
            kind,
            std::string(source_.substr(start_index, index_ - start_index)),
            start,
        });
    }

    void add_error(SourceLocation where, std::string message) {
        diagnostics_.push_back(LexerDiagnostic{where, std::move(message)});
    }

    void skip_trivia() {
        bool consumed = true;
        while (consumed && !is_at_end()) {
            consumed = false;
            while (!is_at_end() && (peek() == ' ' || peek() == '\t' || peek() == '\r' || peek() == '\n')) {
                advance();
                consumed = true;
            }

            if (peek() == '/' && peek_next() == '/') {
                while (!is_at_end() && peek() != '\n') {
                    advance();
                }
                consumed = true;
            }
        }
    }

    void scan_token() {
        const auto start = location_;
        const auto start_index = index_;
        const char ch = advance();

        if (is_identifier_start(ch)) {
            scan_identifier(start, start_index);
            return;
        }

        if (std::isdigit(static_cast<unsigned char>(ch))) {
            scan_integer(start, start_index, ch);
            return;
        }

        switch (ch) {
            case '(': add_token(TokenKind::LParen, start, start_index); break;
            case ')': add_token(TokenKind::RParen, start, start_index); break;
            case '{': add_token(TokenKind::LBrace, start, start_index); break;
            case '}': add_token(TokenKind::RBrace, start, start_index); break;
            case ',': add_token(TokenKind::Comma, start, start_index); break;
            case ';': add_token(TokenKind::Semi, start, start_index); break;
            case ':': add_token(TokenKind::Colon, start, start_index); break;
            case '.': add_token(TokenKind::Dot, start, start_index); break;
            case '+': add_token(TokenKind::Plus, start, start_index); break;
            case '-': add_token(TokenKind::Minus, start, start_index); break;
            case '*': add_token(TokenKind::Star, start, start_index); break;
            case '/': add_token(TokenKind::Slash, start, start_index); break;
            case '%': add_token(TokenKind::Percent, start, start_index); break;
            case '=': add_token(match('=') ? TokenKind::Eq : TokenKind::Assign, start, start_index); break;
            case '!': add_token(match('=') ? TokenKind::Ne : TokenKind::Not, start, start_index); break;
            case '<': add_token(match('=') ? TokenKind::Le : TokenKind::Lt, start, start_index); break;
            case '>': add_token(match('=') ? TokenKind::Ge : TokenKind::Gt, start, start_index); break;
            case '&':
                if (match('&')) {
                    add_token(TokenKind::And, start, start_index);
                } else {
                    add_error(start, "unexpected character '&'; did you mean '&&'?");
                }
                break;
            case '|':
                if (match('|')) {
                    add_token(TokenKind::Or, start, start_index);
                } else {
                    add_error(start, "unexpected character '|'; did you mean '||'?");
                }
                break;
            default:
                add_error(start, std::string("unexpected character '") + ch + "'");
                break;
        }
    }

    void scan_identifier(SourceLocation start, std::size_t start_index) {
        while (!is_at_end() && is_identifier_continue(peek())) {
            advance();
        }

        const auto text = source_.substr(start_index, index_ - start_index);
        const auto found = keywords().find(text);
        add_token(found == keywords().end() ? TokenKind::Identifier : found->second, start, start_index);
    }

    void scan_integer(SourceLocation start, std::size_t start_index, char first) {
        if (first == '0' && !is_at_end() && std::isdigit(static_cast<unsigned char>(peek()))) {
            while (!is_at_end() && std::isdigit(static_cast<unsigned char>(peek()))) {
                advance();
            }
            add_error(start, "invalid integer literal with leading zero");
            return;
        }

        while (!is_at_end() && std::isdigit(static_cast<unsigned char>(peek()))) {
            advance();
        }

        add_token(TokenKind::IntLiteral, start, start_index);
    }

    std::string_view source_;
    std::size_t index_ = 0;
    SourceLocation location_{};
    std::vector<Token> tokens_;
    std::vector<LexerDiagnostic> diagnostics_;
};

}  // namespace

std::string_view token_kind_name(TokenKind kind) {
    switch (kind) {
        case TokenKind::EndOfFile: return "EndOfFile";
        case TokenKind::Identifier: return "Identifier";
        case TokenKind::IntLiteral: return "IntLiteral";
        case TokenKind::KwClass: return "KwClass";
        case TokenKind::KwExtends: return "KwExtends";
        case TokenKind::KwIf: return "KwIf";
        case TokenKind::KwElse: return "KwElse";
        case TokenKind::KwWhile: return "KwWhile";
        case TokenKind::KwReturn: return "KwReturn";
        case TokenKind::KwBreak: return "KwBreak";
        case TokenKind::KwContinue: return "KwContinue";
        case TokenKind::KwTrue: return "KwTrue";
        case TokenKind::KwFalse: return "KwFalse";
        case TokenKind::KwInt: return "KwInt";
        case TokenKind::KwBool: return "KwBool";
        case TokenKind::KwVoid: return "KwVoid";
        case TokenKind::KwThis: return "KwThis";
        case TokenKind::KwSuper: return "KwSuper";
        case TokenKind::KwNull: return "KwNull";
        case TokenKind::LParen: return "LParen";
        case TokenKind::RParen: return "RParen";
        case TokenKind::LBrace: return "LBrace";
        case TokenKind::RBrace: return "RBrace";
        case TokenKind::Comma: return "Comma";
        case TokenKind::Semi: return "Semi";
        case TokenKind::Colon: return "Colon";
        case TokenKind::Dot: return "Dot";
        case TokenKind::Plus: return "Plus";
        case TokenKind::Minus: return "Minus";
        case TokenKind::Star: return "Star";
        case TokenKind::Slash: return "Slash";
        case TokenKind::Percent: return "Percent";
        case TokenKind::Assign: return "Assign";
        case TokenKind::Eq: return "Eq";
        case TokenKind::Ne: return "Ne";
        case TokenKind::Lt: return "Lt";
        case TokenKind::Le: return "Le";
        case TokenKind::Gt: return "Gt";
        case TokenKind::Ge: return "Ge";
        case TokenKind::And: return "And";
        case TokenKind::Or: return "Or";
        case TokenKind::Not: return "Not";
    }
    return "Unknown";
}

LexResult lex(std::string_view source) {
    return Lexer(source).run();
}

}  // namespace adipy
```

- [ ] **Step 2: Run lexer tests**

Run: `make test`

Expected: PASS with `all tests passed`.

- [ ] **Step 3: Commit**

```bash
git add src/frontend/lexer.cpp
git commit -m "feat: implement handwritten lexer"
```

## Task 5: CLI Entry Point

**Files:**
- Create: `src/cli/main.cpp`

- [ ] **Step 1: Create CLI implementation**

Create `src/cli/main.cpp`:

```cpp
#include "adipy/lexer.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace {

void print_usage(std::ostream& out) {
    out << "usage: adipy <lex|parse> <path>\n";
}

bool read_file(const std::string& path, std::string& out) {
    std::ifstream file(path);
    if (!file) {
        return false;
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();
    out = buffer.str();
    return true;
}

int lex_file(const std::string& path) {
    std::string source;
    if (!read_file(path, source)) {
        std::cerr << "error: unable to read file '" << path << "'\n";
        return 1;
    }

    const auto result = adipy::lex(source);
    if (!result.ok()) {
        const auto& diagnostic = result.diagnostics.front();
        std::cerr << diagnostic.location.line << ':' << diagnostic.location.column
                  << ": error: " << diagnostic.message << '\n';
        return 1;
    }

    for (const auto& token : result.tokens) {
        std::cout << token.location.line << ':' << token.location.column << ' '
                  << adipy::token_kind_name(token.kind) << " \""
                  << token.lexeme << "\"\n";
    }

    return 0;
}

}  // namespace

int main(int argc, char** argv) {
    if (argc != 3) {
        print_usage(std::cerr);
        return 1;
    }

    const std::string command = argv[1];
    if (command == "lex") {
        return lex_file(argv[2]);
    }

    print_usage(std::cerr);
    return 1;
}
```

- [ ] **Step 2: Build CLI**

Run: `make`

Expected: PASS and create `build/adipy`.

- [ ] **Step 3: Smoke test CLI manually**

Run:

```bash
printf 'class Point { int x; }\n' > /tmp/adipy-smoke.pj
./build/adipy lex /tmp/adipy-smoke.pj
```

Expected output includes:

```text
1:1 KwClass "class"
1:7 Identifier "Point"
1:13 LBrace "{"
1:15 KwInt "int"
1:19 Identifier "x"
1:20 Semi ";"
1:22 RBrace "}"
2:1 EndOfFile ""
```

- [ ] **Step 4: Verify CLI error behavior**

Run: `./build/adipy`

Expected: nonzero exit and `usage: adipy <lex|parse> <path>` on stderr.

Run: `./build/adipy parse /tmp/adipy-smoke.pj`

Expected: zero exit after parser support is added.

Run: `./build/adipy lex /tmp/does-not-exist.pj`

Expected: nonzero exit and `error: unable to read file '/tmp/does-not-exist.pj'` on stderr.

Run:

```bash
printf '@\n' > /tmp/adipy-bad.pj
./build/adipy lex /tmp/adipy-bad.pj
```

Expected: nonzero exit and `1:1: error: unexpected character '@'` on stderr.

- [ ] **Step 5: Run tests**

Run: `make test`

Expected: PASS with `all tests passed`.

- [ ] **Step 6: Commit**

```bash
git add src/cli/main.cpp
git commit -m "feat: add lexer cli command"
```

## Task 6: Final Verification

**Files:**
- Modify only if verification reveals a defect in files from earlier tasks

- [ ] **Step 1: Clean rebuild**

Run:

```bash
make clean
make
make test
```

Expected:

- `make clean` removes `build/`
- `make` builds `build/adipy`
- `make test` prints `all tests passed`

- [ ] **Step 2: Check git status**

Run: `git status --short`

Expected: clean worktree, except for intentionally ignored local design specs under `docs/superpowers/specs/`.

- [ ] **Step 3: Commit any verification fixes**

If files changed during verification, commit them:

```bash
git add Makefile include/adipy src tests .gitignore
git commit -m "fix: stabilize lexer scaffold"
```

If no files changed, do not create an empty commit.

## Self-Review Notes

- Spec coverage: Makefile, CLI, token model, handwritten lexer, diagnostics, EOF token, tests, and `.gitignore` are covered by Tasks 1-6.
- Scope check: Parser, AST, semantic analysis, IR, runtime, and JIT are excluded from this plan.
- Type consistency: Public API names are `SourceLocation`, `TokenKind`, `Token`, `LexerDiagnostic`, `LexResult`, `lex`, and `token_kind_name` across all tasks.
