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
