#include <exception>
#include <iostream>

void run_lexer_tests();
void run_parser_tests();

int main() {
    try {
        run_lexer_tests();
        run_parser_tests();
    } catch (const std::exception& error) {
        std::cerr << "test failure: " << error.what() << '\n';
        return 1;
    }

    std::cout << "all tests passed\n";
    return 0;
}
