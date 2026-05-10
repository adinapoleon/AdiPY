#pragma once

#include <cstddef>

namespace pyjit {

struct SourceLocation {
    std::size_t line = 1;
    std::size_t column = 1;
};

}  // namespace pyjit
