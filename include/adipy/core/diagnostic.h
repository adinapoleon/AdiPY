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
