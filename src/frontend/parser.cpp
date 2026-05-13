#include "adipy/parser.h"

#include <initializer_list>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "adipy/lexer.h"
#include "adipy/token.h"

namespace adipy {
namespace {

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens) : tokens_(tokens) {}

    ParseResult run() {
        try {
            Program program;
            while (!is_at_end()) {
                program.declarations.push_back(parse_declaration());
            }
            return ParseResult{std::move(program), std::move(diagnostics_)};
        } catch (const ParseFailure&) {
            return ParseResult{Program{}, std::move(diagnostics_)};
        }
    }

private:
    struct ParseFailure {};

    TopLevelDecl parse_declaration() {
        if (match(TokenKind::KwClass)) {
            return wrap_decl(previous().location, parse_class_declaration(previous().location));
        }

        const auto type = parse_type_name();
        const auto name = consume(TokenKind::Identifier, "expected declaration name");
        if (match(TokenKind::LParen)) {
            return wrap_decl(type.location, parse_function_declaration(type, name));
        }
        return wrap_decl(type.location, parse_variable_declaration(type, name));
    }

    ClassDecl parse_class_declaration(SourceLocation start) {
        const auto name = consume(TokenKind::Identifier, "expected class name");

        std::optional<std::string> base_name;
        if (match(TokenKind::KwExtends) || match(TokenKind::Colon)) {
            base_name = consume(TokenKind::Identifier, "expected base class name").lexeme;
        }

        consume(TokenKind::LBrace, "expected '{' before class body");

        std::vector<ClassMember> members;
        while (!check(TokenKind::RBrace) && !is_at_end()) {
            const auto type = parse_type_name();
            const auto member_name = consume(TokenKind::Identifier, "expected member name");
            if (match(TokenKind::LParen)) {
                members.push_back(
                    wrap_member(type.location, parse_method_declaration(type, member_name)));
            } else {
                members.push_back(
                    wrap_member(type.location, parse_field_declaration(type, member_name)));
            }
        }

        consume(TokenKind::RBrace, "expected '}' after class body");
        return ClassDecl{name.lexeme, std::move(base_name), std::move(members), start};
    }

    FunctionDecl parse_function_declaration(const TypeName& return_type, const Token& name) {
        auto parameters = parse_parameter_list();
        consume(TokenKind::LBrace, "expected '{' before function body");
        auto body = parse_block_body(previous().location);
        return FunctionDecl{return_type, name.lexeme, std::move(parameters), std::move(body),
                            return_type.location};
    }

    MethodDecl parse_method_declaration(const TypeName& return_type, const Token& name) {
        auto parameters = parse_parameter_list();
        consume(TokenKind::LBrace, "expected '{' before method body");
        auto body = parse_block_body(previous().location);
        return MethodDecl{return_type, name.lexeme, std::move(parameters), std::move(body),
                          return_type.location};
    }

    FieldDecl parse_field_declaration(const TypeName& type, const Token& name) {
        consume(TokenKind::Semi, "expected ';' after field declaration");
        return FieldDecl{type, name.lexeme, type.location};
    }

    VarDeclStmt parse_variable_declaration(const TypeName& type, const Token& name) {
        ExprPtr initializer;
        if (match(TokenKind::Assign)) {
            initializer = parse_expression();
        }
        consume(TokenKind::Semi, "expected ';' after variable declaration");
        return VarDeclStmt{type, name.lexeme, std::move(initializer), type.location};
    }

    std::vector<Parameter> parse_parameter_list() {
        std::vector<Parameter> parameters;
        if (!check(TokenKind::RParen)) {
            do {
                const auto type = parse_type_name();
                const auto name = consume(TokenKind::Identifier, "expected parameter name");
                parameters.push_back(Parameter{type, name.lexeme, type.location});
            } while (match(TokenKind::Comma));
        }
        consume(TokenKind::RParen, "expected ')' after parameter list");
        return parameters;
    }

    BlockStmt parse_block_body(SourceLocation) {
        std::vector<StmtPtr> statements;
        while (!check(TokenKind::RBrace) && !is_at_end()) {
            statements.push_back(parse_statement());
        }
        consume(TokenKind::RBrace, "expected '}' after block");
        return BlockStmt{std::move(statements)};
    }

    StmtPtr parse_statement() {
        if (match(TokenKind::LBrace)) {
            return wrap_stmt(previous().location, parse_block_body(previous().location));
        }
        if (match(TokenKind::KwIf)) {
            return parse_if_statement(previous().location);
        }
        if (match(TokenKind::KwWhile)) {
            return parse_while_statement(previous().location);
        }
        if (match(TokenKind::KwReturn)) {
            return parse_return_statement(previous().location);
        }
        if (match(TokenKind::KwBreak)) {
            consume(TokenKind::Semi, "expected ';' after break");
            return wrap_stmt(previous().location, BreakStmt{});
        }
        if (match(TokenKind::KwContinue)) {
            consume(TokenKind::Semi, "expected ';' after continue");
            return wrap_stmt(previous().location, ContinueStmt{});
        }
        if (looks_like_variable_declaration()) {
            const auto type = parse_type_name();
            const auto name = consume(TokenKind::Identifier, "expected variable name");
            return wrap_stmt(type.location, parse_variable_declaration(type, name));
        }

        auto expression = parse_expression();
        const auto location = expression->location;
        consume(TokenKind::Semi, "expected ';' after expression");
        return wrap_stmt(location, ExprStmt{std::move(expression)});
    }

    StmtPtr parse_if_statement(SourceLocation start) {
        consume(TokenKind::LParen, "expected '(' after if");
        auto condition = parse_expression();
        consume(TokenKind::RParen, "expected ')' after if condition");
        auto then_branch = parse_statement();

        StmtPtr else_branch;
        if (match(TokenKind::KwElse)) {
            else_branch = parse_statement();
        }

        return wrap_stmt(start, IfStmt{std::move(condition), std::move(then_branch),
                                       std::move(else_branch)});
    }

    StmtPtr parse_while_statement(SourceLocation start) {
        consume(TokenKind::LParen, "expected '(' after while");
        auto condition = parse_expression();
        consume(TokenKind::RParen, "expected ')' after while condition");
        auto body = parse_statement();
        return wrap_stmt(start, WhileStmt{std::move(condition), std::move(body)});
    }

    StmtPtr parse_return_statement(SourceLocation start) {
        ExprPtr value;
        if (!check(TokenKind::Semi)) {
            value = parse_expression();
        }
        consume(TokenKind::Semi, "expected ';' after return");
        return wrap_stmt(start, ReturnStmt{std::move(value)});
    }

    ExprPtr parse_expression() {
        return parse_assignment();
    }

    ExprPtr parse_assignment() {
        auto target = parse_or();
        if (!match(TokenKind::Assign)) {
            return target;
        }

        if (!is_assignable(*target)) {
            fail(target->location, "invalid assignment target");
        }

        const auto location = target->location;
        auto value = parse_assignment();
        return wrap_expr(location, AssignExpr{std::move(target), std::move(value)});
    }

    ExprPtr parse_or() {
        auto expression = parse_and();
        while (match(TokenKind::Or)) {
            const auto location = expression->location;
            auto right = parse_and();
            expression = wrap_expr(
                location, BinaryExpr{BinaryOp::Or, std::move(expression), std::move(right)});
        }
        return expression;
    }

    ExprPtr parse_and() {
        auto expression = parse_equality();
        while (match(TokenKind::And)) {
            const auto location = expression->location;
            auto right = parse_equality();
            expression = wrap_expr(
                location, BinaryExpr{BinaryOp::And, std::move(expression), std::move(right)});
        }
        return expression;
    }

    ExprPtr parse_equality() {
        auto expression = parse_comparison();
        while (match({TokenKind::Eq, TokenKind::Ne})) {
            const auto op = previous().kind == TokenKind::Eq ? BinaryOp::Equal : BinaryOp::NotEqual;
            const auto location = expression->location;
            auto right = parse_comparison();
            expression = wrap_expr(location, BinaryExpr{op, std::move(expression), std::move(right)});
        }
        return expression;
    }

    ExprPtr parse_comparison() {
        auto expression = parse_addition();
        while (match({TokenKind::Lt, TokenKind::Le, TokenKind::Gt, TokenKind::Ge})) {
            BinaryOp op = BinaryOp::Less;
            switch (previous().kind) {
                case TokenKind::Lt: op = BinaryOp::Less; break;
                case TokenKind::Le: op = BinaryOp::LessEqual; break;
                case TokenKind::Gt: op = BinaryOp::Greater; break;
                case TokenKind::Ge: op = BinaryOp::GreaterEqual; break;
                default: break;
            }
            const auto location = expression->location;
            auto right = parse_addition();
            expression = wrap_expr(location, BinaryExpr{op, std::move(expression), std::move(right)});
        }
        return expression;
    }

    ExprPtr parse_addition() {
        auto expression = parse_multiplication();
        while (match({TokenKind::Plus, TokenKind::Minus})) {
            const auto op =
                previous().kind == TokenKind::Plus ? BinaryOp::Add : BinaryOp::Subtract;
            const auto location = expression->location;
            auto right = parse_multiplication();
            expression = wrap_expr(location, BinaryExpr{op, std::move(expression), std::move(right)});
        }
        return expression;
    }

    ExprPtr parse_multiplication() {
        auto expression = parse_unary();
        while (match({TokenKind::Star, TokenKind::Slash, TokenKind::Percent})) {
            BinaryOp op = BinaryOp::Multiply;
            switch (previous().kind) {
                case TokenKind::Star: op = BinaryOp::Multiply; break;
                case TokenKind::Slash: op = BinaryOp::Divide; break;
                case TokenKind::Percent: op = BinaryOp::Modulo; break;
                default: break;
            }
            const auto location = expression->location;
            auto right = parse_unary();
            expression = wrap_expr(location, BinaryExpr{op, std::move(expression), std::move(right)});
        }
        return expression;
    }

    ExprPtr parse_unary() {
        if (match({TokenKind::Not, TokenKind::Minus})) {
            const auto op =
                previous().kind == TokenKind::Not ? UnaryOp::Not : UnaryOp::Negate;
            const auto location = previous().location;
            auto operand = parse_unary();
            return wrap_expr(location, UnaryExpr{op, std::move(operand)});
        }
        return parse_postfix();
    }

    ExprPtr parse_postfix() {
        auto expression = parse_primary();
        while (true) {
            if (match(TokenKind::LParen)) {
                auto arguments = parse_argument_list();
                const auto location = expression->location;
                expression =
                    wrap_expr(location, CallExpr{std::move(expression), std::move(arguments)});
                continue;
            }

            if (match(TokenKind::Dot)) {
                const auto member = consume(TokenKind::Identifier, "expected member name after '.'");
                const auto location = expression->location;
                expression = wrap_expr(
                    location, MemberExpr{std::move(expression), member.lexeme, member.location});
                continue;
            }

            break;
        }
        return expression;
    }

    std::vector<ExprPtr> parse_argument_list() {
        std::vector<ExprPtr> arguments;
        if (!check(TokenKind::RParen)) {
            do {
                arguments.push_back(parse_expression());
            } while (match(TokenKind::Comma));
        }
        consume(TokenKind::RParen, "expected ')' after arguments");
        return arguments;
    }

    ExprPtr parse_primary() {
        if (match(TokenKind::IntLiteral)) {
            return wrap_expr(previous().location, IntLiteralExpr{previous().lexeme});
        }
        if (match(TokenKind::KwTrue)) {
            return wrap_expr(previous().location, BoolLiteralExpr{true});
        }
        if (match(TokenKind::KwFalse)) {
            return wrap_expr(previous().location, BoolLiteralExpr{false});
        }
        if (match(TokenKind::KwNull)) {
            return wrap_expr(previous().location, NullLiteralExpr{});
        }
        if (match(TokenKind::KwThis)) {
            return wrap_expr(previous().location, ThisExpr{});
        }
        if (match(TokenKind::KwSuper)) {
            return wrap_expr(previous().location, SuperExpr{});
        }
        if (match(TokenKind::Identifier)) {
            return wrap_expr(previous().location, VarExpr{previous().lexeme});
        }
        if (match(TokenKind::LParen)) {
            auto expression = parse_expression();
            consume(TokenKind::RParen, "expected ')' after expression");
            return expression;
        }

        fail(peek().location, "expected expression");
        return nullptr;
    }

    TypeName parse_type_name() {
        if (match(TokenKind::KwInt)) {
            return TypeName{TypeName::Kind::Int, "int", previous().location};
        }
        if (match(TokenKind::KwBool)) {
            return TypeName{TypeName::Kind::Bool, "bool", previous().location};
        }
        if (match(TokenKind::KwVoid)) {
            return TypeName{TypeName::Kind::Void, "void", previous().location};
        }
        if (match(TokenKind::Identifier)) {
            return TypeName{TypeName::Kind::Named, previous().lexeme, previous().location};
        }

        fail(peek().location, "expected type name");
        return TypeName{TypeName::Kind::Named, "", peek().location};
    }

    bool looks_like_variable_declaration() const {
        if (check(TokenKind::KwInt) || check(TokenKind::KwBool) || check(TokenKind::KwVoid)) {
            return peek_next().kind == TokenKind::Identifier;
        }
        return check(TokenKind::Identifier) && peek_next().kind == TokenKind::Identifier;
    }

    bool is_assignable(const Expr& expression) const {
        return std::holds_alternative<VarExpr>(expression.node) ||
               std::holds_alternative<MemberExpr>(expression.node);
    }

    bool match(TokenKind kind) {
        if (!check(kind)) {
            return false;
        }
        advance();
        return true;
    }

    bool match(std::initializer_list<TokenKind> kinds) {
        for (const auto kind : kinds) {
            if (check(kind)) {
                advance();
                return true;
            }
        }
        return false;
    }

    const Token& consume(TokenKind kind, std::string_view message) {
        if (check(kind)) {
            return advance();
        }
        fail(peek().location, message);
        return peek();
    }

    void fail(SourceLocation location, std::string_view message) {
        diagnostics_.push_back(ParserDiagnostic{location, std::string(message)});
        throw ParseFailure{};
    }

    const Token& advance() {
        if (!is_at_end()) {
            ++current_;
        }
        return previous();
    }

    bool check(TokenKind kind) const {
        if (is_at_end()) {
            return kind == TokenKind::EndOfFile;
        }
        return peek().kind == kind;
    }

    bool is_at_end() const {
        return peek().kind == TokenKind::EndOfFile;
    }

    const Token& peek() const {
        return tokens_[current_];
    }

    const Token& peek_next() const {
        if (current_ + 1 >= tokens_.size()) {
            return tokens_.back();
        }
        return tokens_[current_ + 1];
    }

    const Token& previous() const {
        return tokens_[current_ - 1];
    }

    template <typename Node>
    ExprPtr wrap_expr(SourceLocation location, Node node) {
        return std::make_unique<Expr>(Expr{location, std::move(node)});
    }

    template <typename Node>
    StmtPtr wrap_stmt(SourceLocation location, Node node) {
        return std::make_unique<Stmt>(Stmt{location, std::move(node)});
    }

    template <typename Node>
    TopLevelDecl wrap_decl(SourceLocation location, Node node) {
        return TopLevelDecl{location, std::move(node)};
    }

    template <typename Node>
    ClassMember wrap_member(SourceLocation location, Node node) {
        return ClassMember{location, std::move(node)};
    }

    const std::vector<Token>& tokens_;
    std::size_t current_ = 0;
    std::vector<ParserDiagnostic> diagnostics_;
};

}  // namespace

ParseResult parse(std::string_view source) {
    const auto lex_result = lex(source);
    if (!lex_result.ok()) {
        std::vector<ParserDiagnostic> diagnostics;
        for (const auto& diagnostic : lex_result.diagnostics) {
            diagnostics.push_back(ParserDiagnostic{diagnostic.location, diagnostic.message});
        }
        return ParseResult{Program{}, std::move(diagnostics)};
    }

    return Parser(lex_result.tokens).run();
}

}  // namespace adipy
