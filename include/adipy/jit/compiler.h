#pragma once

#include "adipy/ir/module.h"

namespace adipy::jit {

// Reserves the ownership boundary for compiled code generation and native entry management.
struct Compiler {};

void enqueue_module(Compiler& compiler, const adipy::ir::Module& module);

}  // namespace adipy::jit
