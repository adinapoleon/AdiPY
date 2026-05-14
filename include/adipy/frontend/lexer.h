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
