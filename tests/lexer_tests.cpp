#include "pyjit/lexer.h"

#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

using pyjit::TokenKind;

void require(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void require_ok(const pyjit::LexResult& result) {
    if (!result.ok()) {
        std::ostringstream out;
        out << result.diagnostics.front().location.line << ':'
            << result.diagnostics.front().location.column << ' '
            << result.diagnostics.front().message;
        throw std::runtime_error(out.str());
    }
}

std::vector<TokenKind> kinds_without_eof(const pyjit::LexResult& result) {
    std::vector<TokenKind> kinds;
    for (const auto& token : result.tokens) {
        if (token.kind != TokenKind::EndOfFile) {
            kinds.push_back(token.kind);
        }
    }
    return kinds;
}

void expect_kinds(const std::string& source, const std::vector<TokenKind>& expected) {
    auto result = pyjit::lex(source);
    require_ok(result);
    require(kinds_without_eof(result) == expected, "token kind sequence mismatch");
}

void test_keywords_and_identifiers() {
    auto result = pyjit::lex("class classic extends iffy if else while return break continue true false int bool void this super null name_1");
    require_ok(result);

    const std::vector<TokenKind> expected = {
        TokenKind::KwClass,
        TokenKind::Identifier,
        TokenKind::KwExtends,
        TokenKind::Identifier,
        TokenKind::KwIf,
        TokenKind::KwElse,
        TokenKind::KwWhile,
        TokenKind::KwReturn,
        TokenKind::KwBreak,
        TokenKind::KwContinue,
        TokenKind::KwTrue,
        TokenKind::KwFalse,
        TokenKind::KwInt,
        TokenKind::KwBool,
        TokenKind::KwVoid,
        TokenKind::KwThis,
        TokenKind::KwSuper,
        TokenKind::KwNull,
        TokenKind::Identifier,
    };

    require(kinds_without_eof(result) == expected, "keyword tokenization mismatch");
    require(result.tokens[1].lexeme == "classic", "identifier lexeme mismatch");
}

void test_comments_and_whitespace() {
    expect_kinds("int x // ignored\nbool y", {
        TokenKind::KwInt,
        TokenKind::Identifier,
        TokenKind::KwBool,
        TokenKind::Identifier,
    });
}

void test_single_character_tokens() {
    expect_kinds("(){} ,;:.+-*/%=<>!", {
        TokenKind::LParen,
        TokenKind::RParen,
        TokenKind::LBrace,
        TokenKind::RBrace,
        TokenKind::Comma,
        TokenKind::Semi,
        TokenKind::Colon,
        TokenKind::Dot,
        TokenKind::Plus,
        TokenKind::Minus,
        TokenKind::Star,
        TokenKind::Slash,
        TokenKind::Percent,
        TokenKind::Assign,
        TokenKind::Lt,
        TokenKind::Gt,
        TokenKind::Not,
    });
}

void test_two_character_tokens() {
    expect_kinds("== != <= >= && ||", {
        TokenKind::Eq,
        TokenKind::Ne,
        TokenKind::Le,
        TokenKind::Ge,
        TokenKind::And,
        TokenKind::Or,
    });
}

void test_locations() {
    auto result = pyjit::lex("int\n  value");
    require_ok(result);
    require(result.tokens[0].location.line == 1, "first token line mismatch");
    require(result.tokens[0].location.column == 1, "first token column mismatch");
    require(result.tokens[1].location.line == 2, "second token line mismatch");
    require(result.tokens[1].location.column == 3, "second token column mismatch");
}

void test_invalid_character() {
    auto result = pyjit::lex("@");
    require(!result.ok(), "invalid character should fail");
    require(result.diagnostics.front().location.line == 1, "invalid character line mismatch");
    require(result.diagnostics.front().location.column == 1, "invalid character column mismatch");
}

void test_standalone_ampersand_and_pipe() {
    auto amp = pyjit::lex("&");
    require(!amp.ok(), "standalone ampersand should fail");
    require(amp.diagnostics.front().message == "unexpected character '&'; did you mean '&&'?", "ampersand diagnostic mismatch");

    auto pipe = pyjit::lex("|");
    require(!pipe.ok(), "standalone pipe should fail");
    require(pipe.diagnostics.front().message == "unexpected character '|'; did you mean '||'?", "pipe diagnostic mismatch");
}

void test_integer_literals() {
    auto result = pyjit::lex("0 12 345");
    require_ok(result);
    require(result.tokens[0].lexeme == "0", "zero literal mismatch");
    require(result.tokens[1].lexeme == "12", "integer literal mismatch");
    require(result.tokens[2].lexeme == "345", "integer literal mismatch");

    auto leading_zero = pyjit::lex("012");
    require(!leading_zero.ok(), "leading zero integer should fail");
    require(leading_zero.diagnostics.front().message == "invalid integer literal with leading zero", "leading zero diagnostic mismatch");
}

}  // namespace

void run_lexer_tests() {
    test_keywords_and_identifiers();
    test_comments_and_whitespace();
    test_single_character_tokens();
    test_two_character_tokens();
    test_locations();
    test_invalid_character();
    test_standalone_ampersand_and_pipe();
    test_integer_literals();
}
