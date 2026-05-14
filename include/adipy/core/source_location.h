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
