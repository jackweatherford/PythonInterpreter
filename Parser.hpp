#ifndef __PARSER_HPP
#define __PARSER_HPP

#include "Token.hpp"
#include "Tokenizer.hpp"
#include "SymTab.hpp"
#include "ArithExpr.hpp"
#include "Statements.hpp"
#include "Functions.hpp"

#include <vector>
#include <iostream>
#include <map>

class Parser {
public:
    Parser(Tokenizer &tokenizer, Functions *&_funcs) : tokenizer{tokenizer}, funcs{_funcs} {}

    Statements *statements();

    // Simple statements
    AssignmentStatement *assignStatement(Token varName);
    PrintStatement *printStatement();
    ArrayOpStatement *arrayOp(Token varName);
    CallStatement *callStatement(Token varName);
    ExprNode *call(Token varName);
    ReturnStatement *returnStatement();

    // Compound statements
    ForStatement *forStatement();
    IfStatement *ifStatement();
    Function *funcDef();

    Subscription *subscription(Token varName);
    std::vector<ExprNode*> array_init();

    Statements *suite();
    std::vector<ExprNode*> testlist();


    std::vector<std::string> parameter_list();

    ExprNode* array_len();

    ExprNode *test();
    ExprNode *or_test();
    ExprNode *and_test();
    ExprNode *not_test();
    ExprNode *comparison();

    ExprNode *arithExpr();
    ExprNode *term();
    ExprNode *factor();
    ExprNode *atom(Token prev, int numHyphens);

private:
    Tokenizer &tokenizer;
    Functions *&funcs;

    void die(std::string where, std::string message, Token &token);

};

#endif //__PARSER_HPP
