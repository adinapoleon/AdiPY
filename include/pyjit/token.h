#pragma once

#include <string>
#include <string_view>

#include "pyjit/source_location.h"

namespace pyjit {

enum class TokenKind {
    EndOfFile,
    Identifier,
    IntLiteral,

    KwClass,
    KwExtends,
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
    KwThis,
    KwSuper,
    KwNull,

    LParen,
    RParen,
    LBrace,
    RBrace,
    Comma,
    Semi,
    Colon,
    Dot,

    Plus,
    Minus,
    Star,
    Slash,
    Percent,
    Assign,
    Eq,
    Ne,
    Lt,
    Le,
    Gt,
    Ge,
    And,
    Or,
    Not,
};

struct Token {
    TokenKind kind;
    std::string lexeme;
    SourceLocation location;
};

std::string_view token_kind_name(TokenKind kind);

}  // namespace pyjit
