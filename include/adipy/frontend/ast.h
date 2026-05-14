#pragma once

#include <memory>
#include <string>
#include <vector>

#include "adipy/core/source_location.h"

namespace adipy::frontend {

struct TypeSyntax;
struct Expr;
struct Stmt;
struct Decl;
struct BlockStmt;

using ExprPtr = std::unique_ptr<Expr>;
using StmtPtr = std::unique_ptr<Stmt>;
using DeclPtr = std::unique_ptr<Decl>;

// Base node type for all frontend syntax objects.
// Keeping source attachment here lets later phases report errors against any node uniformly.
struct Node {
    adipy::core::SourceRange range;
};

// Names a type exactly as the parser saw it.
// Semantic analysis will later reinterpret this spelling into a resolved type graph.
struct TypeSyntax : Node {
    std::string spelling;
};

struct Decl : Node {
    virtual ~Decl();
};

struct Stmt : Node {
    virtual ~Stmt();
};

struct Expr : Node {
    virtual ~Expr();
};

// The root syntax container for one translation unit.
// Future semantic passes begin from this node.
struct Program : Node {
    std::vector<DeclPtr> declarations;
    std::vector<StmtPtr> top_level_statements;
};

// A hybrid callable declaration such as `def<int> add(int x, int y) { ... }`.
struct FunctionDecl : Decl {
    TypeSyntax return_type;
    std::string name;
    std::vector<TypeSyntax> parameter_types;
    std::vector<std::string> parameter_names;
    std::unique_ptr<BlockStmt> body;
};

// A class declaration with optional single inheritance.
struct ClassDecl : Decl {
    std::string name;
    std::string base_name;
    std::vector<DeclPtr> members;
};

// A field declaration using the language's C-style declaration syntax.
struct FieldDecl : Decl {
    TypeSyntax field_type;
    std::string name;
};

// A local declaration statement.
struct VarDeclStmt : Stmt {
    TypeSyntax declared_type;
    std::string name;
    ExprPtr initializer;
};

// A brace-delimited statement suite.
struct BlockStmt : Stmt {
    std::vector<StmtPtr> statements;
};

// A conditional statement.
struct IfStmt : Stmt {
    ExprPtr condition;
    StmtPtr then_branch;
    StmtPtr else_branch;
};

// A while loop statement.
struct WhileStmt : Stmt {
    ExprPtr condition;
    StmtPtr body;
};

// A return statement, optionally carrying a value.
struct ReturnStmt : Stmt {
    ExprPtr value;
};

struct BreakStmt : Stmt {};

struct ContinueStmt : Stmt {};

// A statement that exists for the side effects of evaluating an expression.
struct ExprStmt : Stmt {
    ExprPtr expression;
};

// An assignment expression.
struct AssignExpr : Expr {
    ExprPtr target;
    ExprPtr value;
};

// A binary operator expression.
struct BinaryExpr : Expr {
    std::string op;
    ExprPtr left;
    ExprPtr right;
};

// A unary operator expression.
struct UnaryExpr : Expr {
    std::string op;
    ExprPtr operand;
};

// A call expression.
struct CallExpr : Expr {
    ExprPtr callee;
    std::vector<ExprPtr> arguments;
};

// A member access expression such as `point.x`.
struct MemberExpr : Expr {
    ExprPtr object;
    std::string member_name;
};

// A simple named reference.
struct NameExpr : Expr {
    std::string name;
};

// An integer literal expression.
struct IntegerLiteralExpr : Expr {
    std::string lexeme;
};

// A boolean literal expression.
struct BooleanLiteralExpr : Expr {
    bool value = false;
};

}  // namespace adipy::frontend
