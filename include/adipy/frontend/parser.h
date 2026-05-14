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
