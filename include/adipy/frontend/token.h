#pragma once

#include <string>
#include <string_view>

#include "adipy/core/source_location.h"

namespace adipy::frontend {

// Enumerates the lexical categories required by the line-sensitive, brace-delimited surface language.
// NEWLINE is explicit because semicolons are absent and statement boundaries matter to the parser.
enum class TokenKind {
    EndOfFile,
    Newline,
    Identifier,
    IntegerLiteral,
    BooleanLiteral,

    KwClass,
    KwExtends,
    KwDef,
    KwIf,
    KwElse,
    KwWhile,
    KwReturn,
    KwBreak,
    KwContinue,
    KwTrue,
    KwFalse,
    KwInt,
    KwBool,
    KwVoid,

    LParen,
    RParen,
    LBrace,
    RBrace,
    Less,
    Greater,
    Comma,
    Dot,
    Assign,
    Plus,
    Minus,
    Star,
    Slash,
    Percent,
    PlusEqual,
    MinusEqual,
    EqualEqual,
    BangEqual,
    LessEqual,
    GreaterEqual,
    AndAnd,
    OrOr,
    Bang,
};

// Carries one token and the source slice it came from.
// The parser and diagnostics rely on lexeme preservation for precise error reporting.
struct Token {
    TokenKind kind;
    std::string lexeme;
    adipy::core::SourceRange range;
};

std::string_view token_kind_name(TokenKind kind);

}  // namespace adipy::frontend
