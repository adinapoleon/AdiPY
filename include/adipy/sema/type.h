#pragma once

#include <string>

namespace adipy::sema {

// Represents the semantic type layer that will sit above frontend type syntax.
// This exists now so later passes do not have to invent a home for resolved types.
struct Type {
    std::string debug_name;
};

}  // namespace adipy::sema
