#ifndef EXPRINTER_ARITHEXPR_HPP
#define EXPRINTER_ARITHEXPR_HPP

#include "Token.hpp"
#include "SymTab.hpp"


class Functions;

// Classes in this file define the internal representation of expressions.


// An ExprNode serves as the base class (super class) for an expression.
// It forces the derived classes (subclasses) to implement two functions, print and
// evaluate.
class ExprNode {
public:
    ExprNode(Token token);
    Token token() const;
    Token &token() { return _token; }
    virtual void print() = 0;
    virtual TypeDescriptor* evaluate(SymTab &symTab) = 0;

private:
    Token _token;
};


// An InfixExprNode is useful to represent binary arithmetic operators.
class InfixExprNode: public ExprNode {  // An expression tree node.

public:
    InfixExprNode(Token tk);

    ExprNode *&left();
    ExprNode *&right();
    virtual void print();
    virtual TypeDescriptor* evaluate(SymTab &symTab);

private:
    ExprNode *_left, *_right;
};


// A RelExprNode is useful to represent binary relational operators.
class RelExprNode: public ExprNode {  // An expression tree node.

public:
    RelExprNode(Token tk);

    ExprNode *&left();
    ExprNode *&right();
    virtual void print();
    virtual TypeDescriptor* evaluate(SymTab &symTab);

private:
    ExprNode *_left, *_right;
};

// A BoolExprNode is useful to represent binary boolean operators.
class BoolExprNode: public ExprNode {  // An expression tree node.

public:
    BoolExprNode(Token tk);

    ExprNode *&left();
    ExprNode *&right();
    virtual void print();
    virtual TypeDescriptor* evaluate(SymTab &symTab);

private:
    ExprNode *_left, *_right;
};

// A NotExprNode is useful to represent the unary 'not' operator.
class NotExprNode: public ExprNode {  // An expression tree node.

public:
    NotExprNode(Token tk);

    ExprNode *&right();
    virtual void print();
    virtual TypeDescriptor* evaluate(SymTab &symTab);

private:
    ExprNode *_right;
};

// A Subscription is useful to represent a subscription into an array
class Subscription: public ExprNode {
public:
    Subscription(Token varName, ExprNode* subscript);

    ExprNode *&subscript();
    virtual void print();
    virtual TypeDescriptor* evaluate(SymTab &symTab);

private:
    ExprNode *_subscript;
};

// A Len is useful to represent a length call on an array
class Len: public ExprNode {
public:
    Len(Token varName);
    virtual void print();
    virtual TypeDescriptor* evaluate(SymTab &symTab);
};

class Call: public ExprNode {
public:
    Call(Token funcName, Functions* funcList, std::vector<ExprNode*> args);

    std::vector<ExprNode*> &args();
    virtual void print();
    virtual TypeDescriptor* evaluate(SymTab &symTab);
private:
    std::vector<ExprNode*> _args;
    Functions* _funcList;
};

// WholeNumber is a leaf-node in an expression tree. It corresponds to
// a terminal in the production rules of the grammar that describes the
// syntax of expressions.

class WholeNumber: public ExprNode {
public:
    WholeNumber(Token token);
    virtual void print();
    virtual TypeDescriptor* evaluate(SymTab &symTab);
};

// Variable is a leaf-node in an expression tree. It corresponds to
// a terminal in the production rules of the grammar that describes the
// syntax of expressions.

class Variable: public ExprNode {
public:
    Variable(Token token);
    virtual void print();
    virtual TypeDescriptor* evaluate(SymTab &symTab);
};

// Double is a leaf-node in an expression tree. It corresponds to
// a terminal in the production rules of the grammar that describes the
// syntax of expressions.

class Double: public ExprNode {
public:
    Double(Token token);
    virtual void print();
    virtual TypeDescriptor* evaluate(SymTab &symTab);
};

// String is a leaf-node in an expression tree. It corresponds to
// a terminal in the production rules of the grammar that describes the
// syntax of expressions.

class String: public ExprNode {
public:
    String(Token token);
    virtual void print();
    virtual TypeDescriptor* evaluate(SymTab &symTab);
};

#endif //EXPRINTER_ARITHEXPR_HPP
