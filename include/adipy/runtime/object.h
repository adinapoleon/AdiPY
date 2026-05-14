#pragma once

#include <cstddef>

namespace adipy::runtime {

// Names the future runtime object header and allocation boundary.
// The interpreter and JIT will eventually share this layout contract.
struct ObjectHeader {
    std::size_t size = 0;
};

}  // namespace adipy::runtime
