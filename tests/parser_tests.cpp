#include "adipy/ast.h"
#include "adipy/parser.h"

#include <sstream>
#include <stdexcept>
#include <string>
#include <variant>

namespace {

using adipy::AssignExpr;
using adipy::BinaryExpr;
using adipy::BinaryOp;
using adipy::BlockStmt;
using adipy::BoolLiteralExpr;
using adipy::BreakStmt;
using adipy::CallExpr;
using adipy::ClassDecl;
using adipy::ClassMember;
using adipy::ContinueStmt;
using adipy::Expr;
using adipy::ExprStmt;
using adipy::FieldDecl;
using adipy::FunctionDecl;
using adipy::IfStmt;
using adipy::IntLiteralExpr;
using adipy::MemberExpr;
using adipy::MethodDecl;
using adipy::NullLiteralExpr;
using adipy::ReturnStmt;
using adipy::Stmt;
using adipy::SuperExpr;
using adipy::ThisExpr;
using adipy::TopLevelDecl;
using adipy::TypeName;
using adipy::UnaryExpr;
using adipy::VarDeclStmt;
using adipy::VarExpr;
using adipy::WhileStmt;

void require(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void require_ok(const adipy::ParseResult& result) {
    if (!result.ok()) {
        std::ostringstream out;
        out << result.diagnostics.front().location.line << ':'
            << result.diagnostics.front().location.column << ' '
            << result.diagnostics.front().message;
        throw std::runtime_error(out.str());
    }
}

template <typename T, typename Variant>
const T* expect_variant(const Variant& variant, const std::string& message) {
    const auto* node = std::get_if<T>(&variant);
    require(node != nullptr, message);
    return node;
}

template <typename T>
const T* expect_expr(const Expr& expr, const std::string& message) {
    return expect_variant<T>(expr.node, message);
}

template <typename T>
const T* expect_stmt(const Stmt& stmt, const std::string& message) {
    return expect_variant<T>(stmt.node, message);
}

template <typename T>
const T* expect_decl(const TopLevelDecl& decl, const std::string& message) {
    return expect_variant<T>(decl.node, message);
}

template <typename T>
const T* expect_member(const ClassMember& member, const std::string& message) {
    return expect_variant<T>(member.node, message);
}

void test_parse_function_declaration() {
    const auto result = adipy::parse("int add(int a, int b) { return a + b; }");
    require_ok(result);
    require(result.program.declarations.size() == 1, "expected one declaration");

    const auto* function = expect_decl<FunctionDecl>(
        result.program.declarations.front(), "expected a function declaration");
    require(function->name == "add", "function name mismatch");
    require(function->return_type.kind == TypeName::Kind::Int, "return type kind mismatch");
    require(function->parameters.size() == 2, "parameter count mismatch");
    require(function->parameters[0].name == "a", "first parameter name mismatch");
    require(function->parameters[1].name == "b", "second parameter name mismatch");
    require(function->body.statements.size() == 1, "function body statement count mismatch");

    const auto* return_stmt = expect_stmt<ReturnStmt>(
        *function->body.statements.front(), "expected a return statement");
    require(return_stmt->value != nullptr, "return should have a value");

    const auto* sum_expr = expect_expr<BinaryExpr>(
        *return_stmt->value, "expected binary expression in return");
    require(sum_expr->op == BinaryOp::Add, "expected additive return expression");
    require(expect_expr<VarExpr>(*sum_expr->left, "expected lhs variable")->name == "a",
            "lhs variable mismatch");
    require(expect_expr<VarExpr>(*sum_expr->right, "expected rhs variable")->name == "b",
            "rhs variable mismatch");
}

void test_parse_class_with_extends_and_members() {
    const auto result = adipy::parse(
        "class Child extends Parent { "
        "int x; "
        "int y; "
        "int sum() { return this.x + super.sum(); } "
        "}");
    require_ok(result);
    require(result.program.declarations.size() == 1, "expected one class declaration");

    const auto* class_decl = expect_decl<ClassDecl>(
        result.program.declarations.front(), "expected class declaration");
    require(class_decl->name == "Child", "class name mismatch");
    require(class_decl->base_name.has_value(), "expected base class name");
    require(*class_decl->base_name == "Parent", "base class mismatch");
    require(class_decl->members.size() == 3, "member count mismatch");

    require(expect_member<FieldDecl>(class_decl->members[0], "expected first field")->name == "x",
            "first field mismatch");
    require(expect_member<FieldDecl>(class_decl->members[1], "expected second field")->name == "y",
            "second field mismatch");

    const auto* method = expect_member<MethodDecl>(
        class_decl->members[2], "expected method declaration");
    require(method->name == "sum", "method name mismatch");
    require(method->body.statements.size() == 1, "method body statement count mismatch");

    const auto* return_stmt = expect_stmt<ReturnStmt>(
        *method->body.statements.front(), "expected method return");
    const auto* add_expr = expect_expr<BinaryExpr>(
        *return_stmt->value, "expected addition in method return");
    require(add_expr->op == BinaryOp::Add, "method should add two values");

    const auto* this_member = expect_expr<MemberExpr>(
        *add_expr->left, "expected this member access");
    require(this_member->member == "x", "this member mismatch");
    expect_expr<ThisExpr>(*this_member->object, "expected this receiver");

    const auto* super_call = expect_expr<CallExpr>(
        *add_expr->right, "expected super call");
    const auto* super_member = expect_expr<MemberExpr>(
        *super_call->callee, "expected super member access");
    require(super_member->member == "sum", "super member mismatch");
    expect_expr<SuperExpr>(*super_member->object, "expected super receiver");
}

void test_parse_statements_and_expression_precedence() {
    const auto result = adipy::parse(
        "int main() { "
        "  int value = 0; "
        "  if (flag || count < 1 + 2 * 3) { "
        "    while (ready) { continue; } "
        "  } else { "
        "    break; "
        "  } "
        "  value = obj.method(1, 2 + 3).field; "
        "  return value; "
        "}");
    require_ok(result);

    const auto* function = expect_decl<FunctionDecl>(
        result.program.declarations.front(), "expected main function");
    require(function->body.statements.size() == 4, "main body statement count mismatch");

    const auto* if_stmt = expect_stmt<IfStmt>(
        *function->body.statements[1], "expected if statement");
    const auto* condition = expect_expr<BinaryExpr>(
        *if_stmt->condition, "expected binary condition");
    require(condition->op == BinaryOp::Or, "if condition should use logical or");

    const auto* comparison = expect_expr<BinaryExpr>(
        *condition->right, "expected comparison on right side");
    require(comparison->op == BinaryOp::Less, "expected less-than comparison");

    const auto* additive = expect_expr<BinaryExpr>(
        *comparison->right, "expected additive expression");
    require(additive->op == BinaryOp::Add, "expected addition to bind after comparison");

    const auto* multiplicative = expect_expr<BinaryExpr>(
        *additive->right, "expected multiplicative expression");
    require(multiplicative->op == BinaryOp::Multiply,
            "expected multiplication to bind tighter than addition");

    const auto* then_block = expect_stmt<BlockStmt>(
        *if_stmt->then_branch, "expected block for then branch");
    const auto* while_stmt = expect_stmt<WhileStmt>(
        *then_block->statements.front(), "expected while statement");
    const auto* while_body = expect_stmt<BlockStmt>(
        *while_stmt->body, "expected while body block");
    expect_stmt<ContinueStmt>(*while_body->statements.front(), "expected continue");

    const auto* else_block = expect_stmt<BlockStmt>(
        *if_stmt->else_branch, "expected else block");
    expect_stmt<BreakStmt>(*else_block->statements.front(), "expected break");

    const auto* assignment_stmt = expect_stmt<ExprStmt>(
        *function->body.statements[2], "expected assignment statement");
    const auto* assignment = expect_expr<AssignExpr>(
        *assignment_stmt->expression, "expected assignment expression");
    require(expect_expr<VarExpr>(*assignment->target, "expected assignment target")->name == "value",
            "assignment target mismatch");

    const auto* field_access = expect_expr<MemberExpr>(
        *assignment->value, "expected member access result");
    require(field_access->member == "field", "field member mismatch");

    const auto* call = expect_expr<CallExpr>(
        *field_access->object, "expected call before field access");
    require(call->arguments.size() == 2, "call argument count mismatch");
    require(expect_expr<IntLiteralExpr>(*call->arguments[0], "expected first integer")->lexeme == "1",
            "first call argument mismatch");

    const auto* second_arg = expect_expr<BinaryExpr>(
        *call->arguments[1], "expected additive second argument");
    require(second_arg->op == BinaryOp::Add, "second call argument should be additive");
}

void test_parse_top_level_variable_declaration_and_literals() {
    const auto result = adipy::parse("bool ready = true; int count = 0; Object value = null;");
    require_ok(result);
    require(result.program.declarations.size() == 3, "expected three declarations");

    const auto* ready = expect_decl<VarDeclStmt>(
        result.program.declarations[0], "expected variable declaration");
    require(ready->type.kind == TypeName::Kind::Bool, "bool declaration type mismatch");
    require(ready->name == "ready", "bool declaration name mismatch");
    require(expect_expr<BoolLiteralExpr>(*ready->initializer, "expected bool literal")->value,
            "expected true literal");

    const auto* count = expect_decl<VarDeclStmt>(
        result.program.declarations[1], "expected integer declaration");
    require(count->type.kind == TypeName::Kind::Int, "int declaration type mismatch");
    require(expect_expr<IntLiteralExpr>(*count->initializer, "expected int literal")->lexeme == "0",
            "integer literal mismatch");

    const auto* value = expect_decl<VarDeclStmt>(
        result.program.declarations[2], "expected reference declaration");
    require(value->type.kind == TypeName::Kind::Named, "named type declaration mismatch");
    require(value->type.name == "Object", "named type mismatch");
    expect_expr<NullLiteralExpr>(*value->initializer, "expected null literal");
}

void test_parse_reports_syntax_errors() {
    const auto result = adipy::parse("int main( { return 1; }");
    require(!result.ok(), "malformed function should fail");
    require(result.diagnostics.front().location.line == 1, "diagnostic line mismatch");
    require(result.diagnostics.front().location.column == 11, "diagnostic column mismatch");
}

}  // namespace

void run_parser_tests() {
    test_parse_function_declaration();
    test_parse_class_with_extends_and_members();
    test_parse_statements_and_expression_precedence();
    test_parse_top_level_variable_declaration_and_literals();
    test_parse_reports_syntax_errors();
}
