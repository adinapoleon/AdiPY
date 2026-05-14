#include <string_view>

namespace adipy::tests {

// Declares the frontend test groups without implementing them.
// This keeps the eventual test taxonomy visible without solving the logic now.
void run_lexer_tests();
void run_parser_tests();

}  // namespace adipy::tests

// The real test entrypoint is intentionally deferred.
// This file currently documents the intended harness surface only.
