#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "pyjit/source_location.h"
#include "pyjit/token.h"

namespace pyjit {

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

}  // namespace pyjit
