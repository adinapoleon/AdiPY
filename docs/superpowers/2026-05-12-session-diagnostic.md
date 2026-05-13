# 2026-05-12 Session Diagnostic

## Current State

This session moved the repo past the original "tests first with no parser implementation" failure.

These parser/rename files now exist in the working tree:

- `include/adipy/source_location.h`
- `include/adipy/token.h`
- `include/adipy/lexer.h`
- `include/adipy/ast.h`
- `include/adipy/parser.h`
- `src/frontend/lexer.cpp`
- `src/frontend/parser.cpp`
- `tests/lexer_tests.cpp`
- `tests/parser_tests.cpp`
- `tests/test_main.cpp`
- `Makefile`

The public include tree has been renamed from `include/pyjit/` to `include/adipy/`, and the build outputs in `Makefile` are now `build/adipy` and `build/adipy_tests`.

## Verified Failure

Fresh reproduction used:

```bash
make clean
make test CXXFLAGS='-std=c++20 -Wall -Wextra -Wpedantic -g -fsanitize=address'
```

Observed behavior:

- compilation succeeds
- `build/adipy_tests` is produced
- the test binary crashes at runtime

ASan stack trace points to:

- `src/frontend/parser.cpp:206` in `parse_assignment`
- called while running `test_parse_statements_and_expression_precedence`

Relevant code shape:

```cpp
auto value = parse_assignment();
return wrap_expr(target->location, AssignExpr{std::move(target), std::move(value)});
```

## Root-Cause Hypothesis

The current blocker is **not** "parser and AST are missing."

The current blocker is a likely **move/evaluation-order bug** in the parser implementation:

1. `parse_postfix` had the same class of bug earlier and was already partially fixed by storing `expression->location` before moving `expression`.
2. `parse_assignment` still constructs an `AssignExpr` while also reading `target->location` in the same full expression.
3. The ASan crash at `src/frontend/parser.cpp:206` strongly suggests `target` is being moved before `target->location` is evaluated.

Expected minimal next fix:

```cpp
const auto location = target->location;
auto value = parse_assignment();
return wrap_expr(location, AssignExpr{std::move(target), std::move(value)});
```

Then rerun the same ASan command before doing any other work.

## Secondary Issue

`tests/parser_tests.cpp` emits many `-Wdangling-reference` warnings from helper functions like:

```cpp
const auto& function = expect_decl<FunctionDecl>(...);
```

Those warnings may be false positives, but they should still be cleaned up after the crash is resolved. A safer test helper shape would return pointers and require null checks before dereferencing, rather than returning references from nested variant helpers.

## Incomplete Work

These requested items are **not finished yet**:

- add `adipy parse <path>` CLI command
- add GitHub Actions CI
- finish parser stabilization so `make test` passes
- verify the full repo-wide rename for all remaining docs and strings

## Working-Tree Note

Current `git status --short` before ending the session:

```text
 M Makefile
 M docs/superpowers/plans/2026-05-10-project-scaffold-lexer.md
 D include/pyjit/lexer.h
 D include/pyjit/source_location.h
 D include/pyjit/token.h
 M src/cli/main.cpp
 M src/frontend/lexer.cpp
 M tests/lexer_tests.cpp
 M tests/test_main.cpp
?? docs/superpowers/2026-05-12-session-diagnostic.md
?? docs/superpowers/plans/2026-05-12-adipy-rename-parser-ci.md
?? include/adipy/
?? src/frontend/parser.cpp
?? tests/parser_tests.cpp
```

## Recommended Restart Point

1. Open this diagnostic file.
2. Fix the `parse_assignment` move-order bug first.
3. Re-run the exact ASan command above.
4. Only after the crash is gone, clean up the parser test warnings.
5. Then finish CLI parse support and GitHub Actions.
