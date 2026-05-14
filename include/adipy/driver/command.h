#pragma once

#include <string>
#include <vector>

namespace adipy::driver {

// Enumerates the user-facing driver modes the CLI will eventually support.
enum class CommandKind {
    Lex,
    Parse,
    Run,
    DumpIr,
    JitStats,
};

// Captures one parsed command invocation before execution begins.
struct CommandLine {
    CommandKind kind;
    std::string path;
    std::vector<std::string> arguments;
};

int run_command(const CommandLine& command_line);

}  // namespace adipy::driver
