#pragma once

#include <string>
#include <vector>

namespace adipy::ir {

// Represents a future typed IR container.
// Lowering, interpretation, and JIT compilation will all depend on this boundary.
struct Function {
    std::string name;
};

struct Module {
    std::vector<Function> functions;
};

}  // namespace adipy::ir
