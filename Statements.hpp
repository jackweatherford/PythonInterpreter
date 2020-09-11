#ifndef EXPRINTER_STATEMENTS_HPP
#define EXPRINTER_STATEMENTS_HPP

#include <iostream>
#include <vector>

#include "ArithExpr.hpp"
#include "SymTab.hpp"
#include "Range.hpp"

// The Statement (abstract) class serves as a super class for all statements that
// are defined in the language. Ultimately, statements have to be evaluated.
// Therefore, this class defines evaluate, a pure-virtual function to make
// sure that all subclasses of Statement provide an implementation for this
// function.

class Statement {
public:
    Statement();

    virtual void print() = 0;
    virtual void evaluate(SymTab &symTab) = 0;

};


// Statements is a collection of Statement. For example, all statements in a function
// can be represented by an instance of Statements.

class Statements {
public:
    Statements();

    void addStatement(Statement *statement);
    void evaluate(SymTab &symTab);

    void print();

private:
    std::vector<Statement *> _statements;
};

// AssignmentStatement represents the notion of an lValue having been assigned an rValue.
// The rValue is an expression.

class AssignmentStatement : public Statement {
public:
    AssignmentStatement();
    AssignmentStatement(ExprNode *lhsExpr, ExprNode *rhsExpr);
    AssignmentStatement(ExprNode *lhsExpr, std::vector<ExprNode*> rhsArray);

    ExprNode *&lhsExpression();
    ExprNode *&rhsExpression();
    std::vector<ExprNode*> rhsArray();

    virtual void evaluate(SymTab &symTab);
    virtual void print();

private:
    ExprNode *_lhsExpression;
    ExprNode *_rhsExpression;
    std::vector<ExprNode*> _rhsArray;
};

class PrintStatement : public Statement {
public:
    PrintStatement();
    PrintStatement(std::vector<ExprNode*> list);

    std::vector<ExprNode*> &list();

    virtual void evaluate(SymTab &symTab);
    virtual void print();

private:
    std::vector<ExprNode*> _list;
};

class ForStatement : public Statement {
public:
    ForStatement();
    ForStatement(Token loopVar, std::vector<ExprNode*> rangeArgs, Statements *body);

    Token &loopVar();
    std::vector<ExprNode*> &rangeArgs();
    Statements *&body();

    virtual void evaluate(SymTab &symTab);
    virtual void print();

private:
    Token _loopVar;
    std::vector<ExprNode*> _rangeArgs;
    Statements *_body;
};

class IfStatement : public Statement {
public:
    IfStatement();
    IfStatement(std::vector<ExprNode*> conditions, std::vector<Statements*> bodies);

    std::vector<ExprNode*> &conditions();
    std::vector<Statements*> &bodies();

    virtual void evaluate(SymTab &symTab);
    virtual void print();

private:
    std::vector<ExprNode*> _conditions;
    std::vector<Statements*> _bodies;
};

class ArrayOpStatement : public Statement {
public:
    ArrayOpStatement();
    ArrayOpStatement(Token varName, ExprNode* arg);

    Token &varName();
    ExprNode *&arg();

    virtual void evaluate(SymTab &symTab);
    virtual void print();
private:
    Token _varName;
    ExprNode* _arg;
};

class CallStatement : public Statement {
public:
    CallStatement(Token funcName, std::vector<std::string> params, std::vector<ExprNode*> args, Statements* body);

    std::vector<ExprNode*> &args();
    std::vector<std::string> &params();
    Statements *&body();

    virtual void evaluate(SymTab &symTab);
    virtual void print();
private:
    Token _funcName;
    std::vector<std::string> _params;
    std::vector<ExprNode*> _args;
    Statements* _body;
};

class ReturnStatement : public Statement {
public:
    ReturnStatement(ExprNode* retVal);

    ExprNode *&retVal();

    virtual void evaluate(SymTab &symTab);
    virtual void print();
private:
    ExprNode *_retVal;
};

#endif //EXPRINTER_STATEMENTS_HPP
