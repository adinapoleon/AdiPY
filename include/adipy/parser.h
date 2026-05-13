#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "adipy/ast.h"
#include "adipy/source_location.h"

namespace adipy {

struct ParserDiagnostic {
    SourceLocation location;
    std::string message;
};

struct ParseResult {
    Program program;
    std::vector<ParserDiagnostic> diagnostics;

    bool ok() const {
        return diagnostics.empty();
    }
};

ParseResult parse(std::string_view source);

}  // namespace adipy
