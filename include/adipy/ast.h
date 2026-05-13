#pragma once

#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "adipy/source_location.h"

namespace adipy {

struct TypeName {
    enum class Kind {
        Int,
        Bool,
        Void,
        Named,
    };

    Kind kind;
    std::string name;
    SourceLocation location;
};

struct Parameter {
    TypeName type;
    std::string name;
    SourceLocation location;
};

struct Expr;
struct Stmt;

using ExprPtr = std::unique_ptr<Expr>;
using StmtPtr = std::unique_ptr<Stmt>;

enum class BinaryOp {
    Add,
    Subtract,
    Multiply,
    Divide,
    Modulo,
    Equal,
    NotEqual,
    Less,
    LessEqual,
    Greater,
    GreaterEqual,
    And,
    Or,
};

enum class UnaryOp {
    Negate,
    Not,
};

struct AssignExpr {
    ExprPtr target;
    ExprPtr value;
};

struct BinaryExpr {
    BinaryOp op;
    ExprPtr left;
    ExprPtr right;
};

struct UnaryExpr {
    UnaryOp op;
    ExprPtr operand;
};

struct CallExpr {
    ExprPtr callee;
    std::vector<ExprPtr> arguments;
};

struct MemberExpr {
    ExprPtr object;
    std::string member;
    SourceLocation member_location;
};

struct VarExpr {
    std::string name;
};

struct ThisExpr {};
struct SuperExpr {};

struct IntLiteralExpr {
    std::string lexeme;
};

struct BoolLiteralExpr {
    bool value;
};

struct NullLiteralExpr {};

struct Expr {
    using Node = std::variant<
        AssignExpr,
        BinaryExpr,
        UnaryExpr,
        CallExpr,
        MemberExpr,
        VarExpr,
        ThisExpr,
        SuperExpr,
        IntLiteralExpr,
        BoolLiteralExpr,
        NullLiteralExpr>;

    SourceLocation location;
    Node node;
};

struct BlockStmt {
    std::vector<StmtPtr> statements;
};

struct IfStmt {
    ExprPtr condition;
    StmtPtr then_branch;
    StmtPtr else_branch;
};

struct WhileStmt {
    ExprPtr condition;
    StmtPtr body;
};

struct ReturnStmt {
    ExprPtr value;
};

struct BreakStmt {};
struct ContinueStmt {};

struct VarDeclStmt {
    TypeName type;
    std::string name;
    ExprPtr initializer;
    SourceLocation location;
};

struct ExprStmt {
    ExprPtr expression;
};

struct Stmt {
    using Node = std::variant<
        BlockStmt,
        IfStmt,
        WhileStmt,
        ReturnStmt,
        BreakStmt,
        ContinueStmt,
        VarDeclStmt,
        ExprStmt>;

    SourceLocation location;
    Node node;
};

struct FieldDecl {
    TypeName type;
    std::string name;
    SourceLocation location;
};

struct MethodDecl {
    TypeName return_type;
    std::string name;
    std::vector<Parameter> parameters;
    BlockStmt body;
    SourceLocation location;
};

struct FunctionDecl {
    TypeName return_type;
    std::string name;
    std::vector<Parameter> parameters;
    BlockStmt body;
    SourceLocation location;
};

struct ClassMember {
    using Node = std::variant<FieldDecl, MethodDecl>;

    SourceLocation location;
    Node node;
};

struct ClassDecl {
    std::string name;
    std::optional<std::string> base_name;
    std::vector<ClassMember> members;
    SourceLocation location;
};

struct TopLevelDecl {
    using Node = std::variant<ClassDecl, FunctionDecl, VarDeclStmt>;

    SourceLocation location;
    Node node;
};

struct Program {
    std::vector<TopLevelDecl> declarations;
};

}  // namespace adipy
