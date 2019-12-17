#include "4-gen_ir.h"

#include "0-driver.h"
#include "util.h"

#define DEBUG

using namespace Limc;
using namespace std;

GenIR::GenIR(Driver &driver) : driver(driver), ir_box() {}

string GenIR::gen_expr(Token &root) {
    auto kind = root.get_kind();
#ifdef DEBUG
    cout << "Gen Expr : `" << kind << "`" << endl;
#endif
    if (kind == "Identifier") {

        // END OF [Identifier]
    } else if (kind == "IndexExpr") {

        // END OF [IndexExpr]
    } else if (kind == "ArithBinaryExpr") {

        // END OF [ArithBinaryExpr]
    } else if (kind == "LogBinaryExpr") {

        // END OF [ArithBinaryExpr]
    } else if (kind == "RelationalExpr") {

        // END OF [RelationalExpr]
    } else if (kind == "TernaryExpr") {

        // END OF [TernaryExpr]
    } else if (kind == "MemberExpr") {

        // END OF [MemberExpr]
    } else if (kind == "CallExpr") {

        // END OF [CallExpr]
    } else if (kind == "AssignmentExpr") {

        // END OF [AssignmentExpr]
    } else if (kind == "CompoundArithAssignmentExpr") {

        // END OF [CompoundArithAssignmentExpr]
    } else if (kind == "CompoundLogAssignmentExpr") {

        // END OF [CompoundLogAssignmentExpr]
    } else if (kind == "ParenthesisExpr") {

        // END OF [ParenthesisExpr]
    } else if (kind == "PrefixExpr") {

        // END OF [PrefixExpr]
    } else if (kind == "PostfixExpr") {

        // END OF [PostfixExpr]
    } else if (
        kind == "IntegerLiteral" || kind == "FloatLiteral" || kind == "CharLiteral" ||
        kind == "StringLiteral") {
        /// TODO
    } else {
        throw runtime_error("Uncaught Token Kind: `" + kind + "`.");
    }

    return "";
}

string GenIR::gen_stmt(Token &root) {
    auto  kind     = root.get_kind();
    auto &children = root.get_children();

#ifdef DEBUG
    cout << "Gen Stmt : `" << kind << "`" << endl;
#endif

    if (kind == "Program") {

        for (auto &child : children) {
            gen_stmt(child);
        }
        // END OF [Program]
    } else if (kind == "BlockStmt") {

        for (auto &child : children) {
            gen_stmt(child);
        }
        // END OF [BlockStmt]
    } else if (kind == "Struct") {
        // PASS
        // END OF [Struct]
    } else if (kind == "GlobalVarDecl" || kind == "LocalVarDecl") {
        auto &ident_list = root.get_child(1).get_children();
        for (auto &ident_node_wrap : ident_list) {
            if (ident_node_wrap.get_kind() == "VarDef") {
                auto &lhs   = ident_node_wrap.get_child(0);
                auto &rhs   = ident_node_wrap.get_child(2);
                auto  left  = gen_expr(lhs);
                auto  right = gen_expr(rhs);
            }
        }
        // END OF [GlobalVarDecl] AND [LocalVarDecl]
    } else if (kind == "FuncDef") {
        for (auto &child : root.get_child(3).get_children()) {
            gen_stmt(child);
        }
        // END OF [FuncDef]
    } else if (kind == "ReturnStmt") {

        // END OF [ReturnStmt]
    } else if (kind == "ContinueStmt") {

        // END OF [ContinueStmt]
    } else if (kind == "BreakStmt") {

        // END OF [BreakStmt]
    } else if (kind == "WhileStmt") {

        // END OF [WhileStmt]
    } else if (kind == "WhileStmt" || kind == "DoWhileStmt") {

        // END OF [DoWhileStmt]
    } else if (kind == "ForStmt") {

        // END OF [ForStmt]
    } else if (kind == "IfStmt") {

        // END OF [IfStmt]
    } else {
        gen_expr(root);
    }

    return "";
}
