#pragma once

#include "adipy/ir/module.h"

namespace adipy::interpreter {

// Marks the future tier-0 execution surface for IR.
// Hotness tracking and debug stepping will eventually hang off this session object.
struct Session {};

void load_module(Session& session, const adipy::ir::Module& module);

}  // namespace adipy::interpreter
