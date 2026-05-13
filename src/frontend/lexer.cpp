#include "adipy/lexer.h"

#include <cctype>
#include <string_view>
#include <unordered_map>
#include <utility>

namespace adipy {
namespace {

bool is_identifier_start(char ch) {
    return std::isalpha(static_cast<unsigned char>(ch)) || ch == '_';
}

bool is_identifier_continue(char ch) {
    return std::isalnum(static_cast<unsigned char>(ch)) || ch == '_';
}

const std::unordered_map<std::string_view, TokenKind>& keywords() {
    static const std::unordered_map<std::string_view, TokenKind> table = {
        {"class", TokenKind::KwClass},
        {"extends", TokenKind::KwExtends},
        {"if", TokenKind::KwIf},
        {"else", TokenKind::KwElse},
        {"while", TokenKind::KwWhile},
        {"return", TokenKind::KwReturn},
        {"break", TokenKind::KwBreak},
        {"continue", TokenKind::KwContinue},
        {"true", TokenKind::KwTrue},
        {"false", TokenKind::KwFalse},
        {"int", TokenKind::KwInt},
        {"bool", TokenKind::KwBool},
        {"void", TokenKind::KwVoid},
        {"this", TokenKind::KwThis},
        {"super", TokenKind::KwSuper},
        {"null", TokenKind::KwNull},
    };
    return table;
}

class Lexer {
public:
    explicit Lexer(std::string_view source) : source_(source) {}

    LexResult run() {
        while (!is_at_end() && diagnostics_.empty()) {
            skip_trivia();
            if (!is_at_end()) {
                scan_token();
            }
        }

        tokens_.push_back(Token{TokenKind::EndOfFile, "", location_});
        return LexResult{tokens_, diagnostics_};
    }

private:
    bool is_at_end() const {
        return index_ >= source_.size();
    }

    char peek() const {
        return is_at_end() ? '\0' : source_[index_];
    }

    char peek_next() const {
        return index_ + 1 >= source_.size() ? '\0' : source_[index_ + 1];
    }

    char advance() {
        const char ch = source_[index_++];
        if (ch == '\n') {
            ++location_.line;
            location_.column = 1;
        } else {
            ++location_.column;
        }
        return ch;
    }

    bool match(char expected) {
        if (is_at_end() || source_[index_] != expected) {
            return false;
        }
        advance();
        return true;
    }

    void add_token(TokenKind kind, SourceLocation start, std::size_t start_index) {
        tokens_.push_back(Token{
            kind,
            std::string(source_.substr(start_index, index_ - start_index)),
            start,
        });
    }

    void add_error(SourceLocation where, std::string message) {
        diagnostics_.push_back(LexerDiagnostic{where, std::move(message)});
    }

    void skip_trivia() {
        bool consumed = true;
        while (consumed && !is_at_end()) {
            consumed = false;
            while (!is_at_end() && (peek() == ' ' || peek() == '\t' || peek() == '\r' || peek() == '\n')) {
                advance();
                consumed = true;
            }

            if (peek() == '/' && peek_next() == '/') {
                while (!is_at_end() && peek() != '\n') {
                    advance();
                }
                consumed = true;
            }
        }
    }

    void scan_token() {
        const auto start = location_;
        const auto start_index = index_;
        const char ch = advance();

        if (is_identifier_start(ch)) {
            scan_identifier(start, start_index);
            return;
        }

        if (std::isdigit(static_cast<unsigned char>(ch))) {
            scan_integer(start, start_index, ch);
            return;
        }

        switch (ch) {
            case '(': add_token(TokenKind::LParen, start, start_index); break;
            case ')': add_token(TokenKind::RParen, start, start_index); break;
            case '{': add_token(TokenKind::LBrace, start, start_index); break;
            case '}': add_token(TokenKind::RBrace, start, start_index); break;
            case ',': add_token(TokenKind::Comma, start, start_index); break;
            case ';': add_token(TokenKind::Semi, start, start_index); break;
            case ':': add_token(TokenKind::Colon, start, start_index); break;
            case '.': add_token(TokenKind::Dot, start, start_index); break;
            case '+': add_token(TokenKind::Plus, start, start_index); break;
            case '-': add_token(TokenKind::Minus, start, start_index); break;
            case '*': add_token(TokenKind::Star, start, start_index); break;
            case '/': add_token(TokenKind::Slash, start, start_index); break;
            case '%': add_token(TokenKind::Percent, start, start_index); break;
            case '=': add_token(match('=') ? TokenKind::Eq : TokenKind::Assign, start, start_index); break;
            case '!': add_token(match('=') ? TokenKind::Ne : TokenKind::Not, start, start_index); break;
            case '<': add_token(match('=') ? TokenKind::Le : TokenKind::Lt, start, start_index); break;
            case '>': add_token(match('=') ? TokenKind::Ge : TokenKind::Gt, start, start_index); break;
            case '&':
                if (match('&')) {
                    add_token(TokenKind::And, start, start_index);
                } else {
                    add_error(start, "unexpected character '&'; did you mean '&&'?");
                }
                break;
            case '|':
                if (match('|')) {
                    add_token(TokenKind::Or, start, start_index);
                } else {
                    add_error(start, "unexpected character '|'; did you mean '||'?");
                }
                break;
            default:
                add_error(start, std::string("unexpected character '") + ch + "'");
                break;
        }
    }

    void scan_identifier(SourceLocation start, std::size_t start_index) {
        while (!is_at_end() && is_identifier_continue(peek())) {
            advance();
        }

        const auto text = source_.substr(start_index, index_ - start_index);
        const auto found = keywords().find(text);
        add_token(found == keywords().end() ? TokenKind::Identifier : found->second, start, start_index);
    }

    void scan_integer(SourceLocation start, std::size_t start_index, char first) {
        if (first == '0' && !is_at_end() && std::isdigit(static_cast<unsigned char>(peek()))) {
            while (!is_at_end() && std::isdigit(static_cast<unsigned char>(peek()))) {
                advance();
            }
            add_error(start, "invalid integer literal with leading zero");
            return;
        }

        while (!is_at_end() && std::isdigit(static_cast<unsigned char>(peek()))) {
            advance();
        }

        add_token(TokenKind::IntLiteral, start, start_index);
    }

    std::string_view source_;
    std::size_t index_ = 0;
    SourceLocation location_{};
    std::vector<Token> tokens_;
    std::vector<LexerDiagnostic> diagnostics_;
};

}  // namespace

std::string_view token_kind_name(TokenKind kind) {
    switch (kind) {
        case TokenKind::EndOfFile: return "EndOfFile";
        case TokenKind::Identifier: return "Identifier";
        case TokenKind::IntLiteral: return "IntLiteral";
        case TokenKind::KwClass: return "KwClass";
        case TokenKind::KwExtends: return "KwExtends";
        case TokenKind::KwIf: return "KwIf";
        case TokenKind::KwElse: return "KwElse";
        case TokenKind::KwWhile: return "KwWhile";
        case TokenKind::KwReturn: return "KwReturn";
        case TokenKind::KwBreak: return "KwBreak";
        case TokenKind::KwContinue: return "KwContinue";
        case TokenKind::KwTrue: return "KwTrue";
        case TokenKind::KwFalse: return "KwFalse";
        case TokenKind::KwInt: return "KwInt";
        case TokenKind::KwBool: return "KwBool";
        case TokenKind::KwVoid: return "KwVoid";
        case TokenKind::KwThis: return "KwThis";
        case TokenKind::KwSuper: return "KwSuper";
        case TokenKind::KwNull: return "KwNull";
        case TokenKind::LParen: return "LParen";
        case TokenKind::RParen: return "RParen";
        case TokenKind::LBrace: return "LBrace";
        case TokenKind::RBrace: return "RBrace";
        case TokenKind::Comma: return "Comma";
        case TokenKind::Semi: return "Semi";
        case TokenKind::Colon: return "Colon";
        case TokenKind::Dot: return "Dot";
        case TokenKind::Plus: return "Plus";
        case TokenKind::Minus: return "Minus";
        case TokenKind::Star: return "Star";
        case TokenKind::Slash: return "Slash";
        case TokenKind::Percent: return "Percent";
        case TokenKind::Assign: return "Assign";
        case TokenKind::Eq: return "Eq";
        case TokenKind::Ne: return "Ne";
        case TokenKind::Lt: return "Lt";
        case TokenKind::Le: return "Le";
        case TokenKind::Gt: return "Gt";
        case TokenKind::Ge: return "Ge";
        case TokenKind::And: return "And";
        case TokenKind::Or: return "Or";
        case TokenKind::Not: return "Not";
    }
    return "Unknown";
}

LexResult lex(std::string_view source) {
    return Lexer(source).run();
}

}  // namespace adipy
