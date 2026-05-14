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
