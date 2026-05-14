#pragma once

#include "adipy/frontend/ast.h"
#include "adipy/ir/module.h"

namespace adipy::ir {

// Defines the AST-to-IR boundary without committing to instruction selection yet.
Module lower_program(const adipy::frontend::Program& program);

}  // namespace adipy::ir
