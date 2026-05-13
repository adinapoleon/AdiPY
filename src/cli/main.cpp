#include "adipy/lexer.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace {

void print_usage(std::ostream& out) {
    out << "usage: adipy lex <path>\n";
}

bool read_file(const std::string& path, std::string& out) {
    std::ifstream file(path);
    if (!file) {
        return false;
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();
    out = buffer.str();
    return true;
}

int lex_file(const std::string& path) {
    std::string source;
    if (!read_file(path, source)) {
        std::cerr << "error: unable to read file '" << path << "'\n";
        return 1;
    }

    const auto result = adipy::lex(source);
    if (!result.ok()) {
        const auto& diagnostic = result.diagnostics.front();
        std::cerr << diagnostic.location.line << ':' << diagnostic.location.column
                  << ": error: " << diagnostic.message << '\n';
        return 1;
    }

    for (const auto& token : result.tokens) {
        std::cout << token.location.line << ':' << token.location.column << ' '
                  << adipy::token_kind_name(token.kind) << " \""
                  << token.lexeme << "\"\n";
    }

    return 0;
}

}  // namespace

int main(int argc, char** argv) {
    if (argc != 3) {
        print_usage(std::cerr);
        return 1;
    }

    const std::string command = argv[1];
    if (command == "lex") {
        return lex_file(argv[2]);
    }

    print_usage(std::cerr);
    return 1;
}
