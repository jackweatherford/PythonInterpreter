#include <iostream>
#include "ArithExpr.hpp"
#include "Functions.hpp"
#include "Globals.hpp"

void die(std::string where, std::string op, TypeDescriptor::types lType, TypeDescriptor::types rType) {
    std::cout << where << " Unsupported operand types for " << op << ": ";
    switch ( lType ) {
        case ( TypeDescriptor::INTEGER ):
            std::cout << "Integer";
            break;
        case ( TypeDescriptor::DOUBLE ):
            std::cout << "Double";
            break;
        case ( TypeDescriptor::STRING ):
            std::cout << "String";
            break;
        case ( TypeDescriptor::ARRAY ):
            std::cout << "Array";
            break;
    }
    std::cout << " and ";
    switch ( rType ) {
        case ( TypeDescriptor::INTEGER ):
            std::cout << "Integer";
            break;
        case ( TypeDescriptor::DOUBLE ):
            std::cout << "Double";
            break;
        case ( TypeDescriptor::STRING ):
            std::cout << "String";
            break;
        case ( TypeDescriptor::ARRAY ):
            std::cout << "Array";
            break;
    }
    std::cout << std::endl;
    exit(1);
}

// ExprNode
ExprNode::ExprNode(Token token): _token{token} {}

Token ExprNode::token() const { return _token; }

// InfixExprNode functions
InfixExprNode::InfixExprNode(Token tk) : ExprNode{tk}, _left(nullptr), _right(nullptr) {}

ExprNode *&InfixExprNode::left() { return _left; }

ExprNode *&InfixExprNode::right() { return _right; }

TypeDescriptor* InfixExprNode::evaluate(SymTab &symTab) {
    // evaluates an infix expression using a post-order traversal of the expression tree.
    TypeDescriptor* lValue = left()->evaluate(symTab);
    TypeDescriptor::types lType = lValue->type();
    TypeDescriptor* rValue = right()->evaluate(symTab);
    TypeDescriptor::types rType = rValue->type();
	if (verbose) {
		std::cout << "InfixExprNode::evaluate: ";
		lValue->print();
		token().print();
		rValue->print();
		std::cout << std::endl;
	}
    if( token().isAdditionOperator() ) {
        if ( lType == TypeDescriptor::INTEGER && rType == TypeDescriptor::INTEGER )
            return new IntegerDescriptor(dynamic_cast<IntegerDescriptor *>(lValue)->value() +
                                         dynamic_cast<IntegerDescriptor *>(rValue)->value());
        else if ( lType == TypeDescriptor::INTEGER && rType == TypeDescriptor::DOUBLE )
            return new DoubleDescriptor(dynamic_cast<IntegerDescriptor*>(lValue)->value() +
                                       dynamic_cast<DoubleDescriptor*>(rValue)->value());
        else if ( lType == TypeDescriptor::DOUBLE && rType == TypeDescriptor::INTEGER )
            return new DoubleDescriptor(dynamic_cast<DoubleDescriptor*>(lValue)->value() +
                                       dynamic_cast<IntegerDescriptor*>(rValue)->value());
        else if ( lType == TypeDescriptor::DOUBLE && rType == TypeDescriptor::DOUBLE )
            return new DoubleDescriptor(dynamic_cast<DoubleDescriptor*>(lValue)->value() +
                                       dynamic_cast<DoubleDescriptor*>(rValue)->value());
        else if ( lType == TypeDescriptor::STRING && rType == TypeDescriptor::STRING )
            return new StringDescriptor(dynamic_cast<StringDescriptor*>(lValue)->value() +
                                        dynamic_cast<StringDescriptor*>(rValue)->value());
        // If operand types are unsupported
        die("InfixExprNode::evaluate", "+", lType, rType);
    }
    else if(token().isSubtractionOperator()) {
        if ( lType == TypeDescriptor::INTEGER && rType == TypeDescriptor::INTEGER )
            return new IntegerDescriptor(dynamic_cast<IntegerDescriptor*>(lValue)->value() -
                                         dynamic_cast<IntegerDescriptor*>(rValue)->value());
        else if ( lType == TypeDescriptor::INTEGER && rType == TypeDescriptor::DOUBLE )
            return new DoubleDescriptor(dynamic_cast<IntegerDescriptor*>(lValue)->value() -
                                       dynamic_cast<DoubleDescriptor*>(rValue)->value());
        else if ( lType == TypeDescriptor::DOUBLE && rType == TypeDescriptor::INTEGER )
            return new DoubleDescriptor(dynamic_cast<DoubleDescriptor*>(lValue)->value() -
                                       dynamic_cast<IntegerDescriptor*>(rValue)->value());
        else if ( lType == TypeDescriptor::DOUBLE && rType == TypeDescriptor::DOUBLE )
            return new DoubleDescriptor(dynamic_cast<DoubleDescriptor*>(lValue)->value() -
                                       dynamic_cast<DoubleDescriptor*>(rValue)->value());
        // If operand types are unsupported
        die("InfixExprNode::evaluate", "-", lType, rType);
    }
    else if(token().isMultiplicationOperator()) {
        if ( lType == TypeDescriptor::INTEGER && rType == TypeDescriptor::INTEGER )
            return new IntegerDescriptor(dynamic_cast<IntegerDescriptor*>(lValue)->value() *
                                         dynamic_cast<IntegerDescriptor*>(rValue)->value());
        else if ( lType == TypeDescriptor::INTEGER && rType == TypeDescriptor::DOUBLE )
            return new DoubleDescriptor(dynamic_cast<IntegerDescriptor*>(lValue)->value() *
                                       dynamic_cast<DoubleDescriptor*>(rValue)->value());
        else if ( lType == TypeDescriptor::DOUBLE && rType == TypeDescriptor::INTEGER )
            return new DoubleDescriptor(dynamic_cast<DoubleDescriptor*>(lValue)->value() *
                                       dynamic_cast<IntegerDescriptor*>(rValue)->value());
        else if ( lType == TypeDescriptor::DOUBLE && rType == TypeDescriptor::DOUBLE )
            return new DoubleDescriptor(dynamic_cast<DoubleDescriptor*>(lValue)->value() *
                                       dynamic_cast<DoubleDescriptor*>(rValue)->value());
        // If operand types are unsupported
        die("InfixExprNode::evaluate", "*", lType, rType);
    }
    else if(token().isDivisionOperator()) {
        if ( ( rType == TypeDescriptor::INTEGER && dynamic_cast<IntegerDescriptor*>(rValue)->value() == 0 ) ||
             ( rType == TypeDescriptor::DOUBLE && dynamic_cast<DoubleDescriptor*>(rValue)->value() == 0 ) ) {
            std::cout << "InfixExprNode::evaluate: divide by zero error\n";
            exit(2);
        }
        if ( lType == TypeDescriptor::INTEGER && rType == TypeDescriptor::INTEGER )
            return new DoubleDescriptor(dynamic_cast<IntegerDescriptor*>(lValue)->value() /
                                      (double) dynamic_cast<IntegerDescriptor*>(rValue)->value());
        else if ( lType == TypeDescriptor::INTEGER && rType == TypeDescriptor::DOUBLE )
            return new DoubleDescriptor(dynamic_cast<IntegerDescriptor*>(lValue)->value() /
                                       dynamic_cast<DoubleDescriptor*>(rValue)->value());
        else if ( lType == TypeDescriptor::DOUBLE && rType == TypeDescriptor::INTEGER )
            return new DoubleDescriptor(dynamic_cast<DoubleDescriptor*>(lValue)->value() /
                                       dynamic_cast<IntegerDescriptor*>(rValue)->value());
        else if ( lType == TypeDescriptor::DOUBLE && rType == TypeDescriptor::DOUBLE )
            return new DoubleDescriptor(dynamic_cast<DoubleDescriptor*>(lValue)->value() /
                                       dynamic_cast<DoubleDescriptor*>(rValue)->value());
        // If operand types are unsupported
        die("InfixExprNode::evaluate", "/", lType, rType);
    }
    else if( token().isModuloOperator() ) {
        if ( lType == TypeDescriptor::INTEGER && rType == TypeDescriptor::INTEGER )
            return new IntegerDescriptor(dynamic_cast<IntegerDescriptor*>(lValue)->value() %
                                         dynamic_cast<IntegerDescriptor*>(rValue)->value());
        // If operand types are unsupported
        die("InfixExprNode::evaluate", "%", lType, rType);
    }
    else if ( token().isIntDivisionOperator() ) {
        if ( lType == TypeDescriptor::INTEGER && rType == TypeDescriptor::INTEGER )
            return new IntegerDescriptor(dynamic_cast<IntegerDescriptor*>(lValue)->value() /
                                         dynamic_cast<IntegerDescriptor*>(rValue)->value());
        else if ( lType == TypeDescriptor::INTEGER && rType == TypeDescriptor::DOUBLE )
            return new DoubleDescriptor((int)(dynamic_cast<IntegerDescriptor*>(lValue)->value() /
                                        dynamic_cast<DoubleDescriptor*>(rValue)->value()));
        else if ( lType == TypeDescriptor::DOUBLE && rType == TypeDescriptor::INTEGER )
            return new DoubleDescriptor((int)(dynamic_cast<DoubleDescriptor*>(lValue)->value() /
                                        dynamic_cast<IntegerDescriptor*>(rValue)->value()));
        else if ( lType == TypeDescriptor::DOUBLE && rType == TypeDescriptor::DOUBLE )
            return new DoubleDescriptor((int)(dynamic_cast<DoubleDescriptor*>(lValue)->value() /
                                        dynamic_cast<DoubleDescriptor*>(rValue)->value()));
        // If operand types are unsupported
        die("InfixExprNode::evaluate", "//", lType, rType);
    }
    else {
        std::cout << "InfixExprNode::evaluate: don't know how to evaluate this operator\n";
        token().print();
        std::cout << std::endl;
        exit(2);
    }

    return nullptr; // Will never reach here
}

void InfixExprNode::print() {
    _left->print();
    token().print();
    _right->print();
}

// RelExprNode
RelExprNode::RelExprNode(Token tk) : ExprNode{tk}, _left(nullptr), _right(nullptr) {}

ExprNode *&RelExprNode::left() { return _left; }

ExprNode *&RelExprNode::right() { return _right; }

TypeDescriptor* RelExprNode::evaluate(SymTab &symTab) {
    // evaluates a relational expression using a post-order traversal of the expression tree.
    TypeDescriptor* lValue = left()->evaluate(symTab);
    TypeDescriptor::types lType = lValue->type();
    TypeDescriptor* rValue = right()->evaluate(symTab);
    TypeDescriptor::types rType = rValue->type();
	if (verbose) {
		std::cout << "RelExprNode::evaluate: ";
		lValue->print();
		token().print();
		rValue->print();
		std::cout << std::endl;
	}
    if (token().isGreaterThanOperator()) {
        if ( lType == TypeDescriptor::INTEGER && rType == TypeDescriptor::INTEGER )
            return new IntegerDescriptor(dynamic_cast<IntegerDescriptor*>(lValue)->value() >
                                         dynamic_cast<IntegerDescriptor*>(rValue)->value());
        else if ( lType == TypeDescriptor::INTEGER && rType == TypeDescriptor::DOUBLE )
            return new IntegerDescriptor(dynamic_cast<IntegerDescriptor*>(lValue)->value() >
                                         dynamic_cast<DoubleDescriptor*>(rValue)->value());
        else if ( lType == TypeDescriptor::DOUBLE && rType == TypeDescriptor::INTEGER )
            return new IntegerDescriptor(dynamic_cast<DoubleDescriptor*>(lValue)->value() >
                                         dynamic_cast<IntegerDescriptor*>(rValue)->value());
        else if ( lType == TypeDescriptor::DOUBLE && rType == TypeDescriptor::DOUBLE )
            return new IntegerDescriptor(dynamic_cast<DoubleDescriptor*>(lValue)->value() >
                                         dynamic_cast<DoubleDescriptor*>(rValue)->value());
        else if ( lType == TypeDescriptor::STRING && rType == TypeDescriptor::STRING )
            return new IntegerDescriptor(dynamic_cast<StringDescriptor*>(lValue)->value() >
                                         dynamic_cast<StringDescriptor*>(rValue)->value());
        // If operand types are unsupported
        die("RelExprNode::evaluate", ">", lType, rType);
    }
    else if (token().isGreaterThanOrEqualToOperator()) {
        if ( lType == TypeDescriptor::INTEGER && rType == TypeDescriptor::INTEGER )
            return new IntegerDescriptor(dynamic_cast<IntegerDescriptor*>(lValue)->value() >=
                                         dynamic_cast<IntegerDescriptor*>(rValue)->value());
        else if ( lType == TypeDescriptor::INTEGER && rType == TypeDescriptor::DOUBLE )
            return new IntegerDescriptor(dynamic_cast<IntegerDescriptor*>(lValue)->value() >=
                                         dynamic_cast<DoubleDescriptor*>(rValue)->value());
        else if ( lType == TypeDescriptor::DOUBLE && rType == TypeDescriptor::INTEGER )
            return new IntegerDescriptor(dynamic_cast<DoubleDescriptor*>(lValue)->value() >=
                                         dynamic_cast<IntegerDescriptor*>(rValue)->value());
        else if ( lType == TypeDescriptor::DOUBLE && rType == TypeDescriptor::DOUBLE )
            return new IntegerDescriptor(dynamic_cast<DoubleDescriptor*>(lValue)->value() >=
                                         dynamic_cast<DoubleDescriptor*>(rValue)->value());
        else if ( lType == TypeDescriptor::STRING && rType == TypeDescriptor::STRING )
            return new IntegerDescriptor(dynamic_cast<StringDescriptor*>(lValue)->value() >=
                                         dynamic_cast<StringDescriptor*>(rValue)->value());
        // If operand types are unsupported
        die("RelExprNode::evaluate", ">=", lType, rType);
    }
    else if (token().isLessThanOperator()) {
        if ( lType == TypeDescriptor::INTEGER && rType == TypeDescriptor::INTEGER )
            return new IntegerDescriptor(dynamic_cast<IntegerDescriptor*>(lValue)->value() <
                                         dynamic_cast<IntegerDescriptor*>(rValue)->value());
        else if ( lType == TypeDescriptor::INTEGER && rType == TypeDescriptor::DOUBLE )
            return new IntegerDescriptor(dynamic_cast<IntegerDescriptor*>(lValue)->value() <
                                         dynamic_cast<DoubleDescriptor*>(rValue)->value());
        else if ( lType == TypeDescriptor::DOUBLE && rType == TypeDescriptor::INTEGER )
            return new IntegerDescriptor(dynamic_cast<DoubleDescriptor*>(lValue)->value() <
                                         dynamic_cast<IntegerDescriptor*>(rValue)->value());
        else if ( lType == TypeDescriptor::DOUBLE && rType == TypeDescriptor::DOUBLE )
            return new IntegerDescriptor(dynamic_cast<DoubleDescriptor*>(lValue)->value() <
                                         dynamic_cast<DoubleDescriptor*>(rValue)->value());
        else if ( lType == TypeDescriptor::STRING && rType == TypeDescriptor::STRING )
            return new IntegerDescriptor(dynamic_cast<StringDescriptor*>(lValue)->value() <
                                         dynamic_cast<StringDescriptor*>(rValue)->value());
        // If operand types are unsupported
        die("RelExprNode::evaluate", "<", lType, rType);
    }
    else if (token().isLessThanOrEqualToOperator()) {
        if ( lType == TypeDescriptor::INTEGER && rType == TypeDescriptor::INTEGER )
            return new IntegerDescriptor(dynamic_cast<IntegerDescriptor*>(lValue)->value() <=
                                         dynamic_cast<IntegerDescriptor*>(rValue)->value());
        else if ( lType == TypeDescriptor::INTEGER && rType == TypeDescriptor::DOUBLE )
            return new IntegerDescriptor(dynamic_cast<IntegerDescriptor*>(lValue)->value() <=
                                         dynamic_cast<DoubleDescriptor*>(rValue)->value());
        else if ( lType == TypeDescriptor::DOUBLE && rType == TypeDescriptor::INTEGER )
            return new IntegerDescriptor(dynamic_cast<DoubleDescriptor*>(lValue)->value() <=
                                         dynamic_cast<IntegerDescriptor*>(rValue)->value());
        else if ( lType == TypeDescriptor::DOUBLE && rType == TypeDescriptor::DOUBLE )
            return new IntegerDescriptor(dynamic_cast<DoubleDescriptor*>(lValue)->value() <=
                                         dynamic_cast<DoubleDescriptor*>(rValue)->value());
        else if ( lType == TypeDescriptor::STRING && rType == TypeDescriptor::STRING )
            return new IntegerDescriptor(dynamic_cast<StringDescriptor*>(lValue)->value() <=
                                         dynamic_cast<StringDescriptor*>(rValue)->value());
        // If operand types are unsupported
        die("RelExprNode::evaluate", "<=", lType, rType);
    }
    else if (token().isEqualityOperator()) {
        if ( lType == TypeDescriptor::INTEGER && rType == TypeDescriptor::INTEGER )
            return new IntegerDescriptor(dynamic_cast<IntegerDescriptor*>(lValue)->value() ==
                                         dynamic_cast<IntegerDescriptor*>(rValue)->value());
        else if ( lType == TypeDescriptor::INTEGER && rType == TypeDescriptor::DOUBLE )
            return new IntegerDescriptor(dynamic_cast<IntegerDescriptor*>(lValue)->value() ==
                                         dynamic_cast<DoubleDescriptor*>(rValue)->value());
        else if ( lType == TypeDescriptor::DOUBLE && rType == TypeDescriptor::INTEGER )
            return new IntegerDescriptor(dynamic_cast<DoubleDescriptor*>(lValue)->value() ==
                                         dynamic_cast<IntegerDescriptor*>(rValue)->value());
        else if ( lType == TypeDescriptor::DOUBLE && rType == TypeDescriptor::DOUBLE )
            return new IntegerDescriptor(dynamic_cast<DoubleDescriptor*>(lValue)->value() ==
                                         dynamic_cast<DoubleDescriptor*>(rValue)->value());
        else if ( lType == TypeDescriptor::STRING && rType == TypeDescriptor::STRING )
            return new IntegerDescriptor(dynamic_cast<StringDescriptor*>(lValue)->value() ==
                                         dynamic_cast<StringDescriptor*>(rValue)->value());
        // If operand types are unsupported
        die("RelExprNode::evaluate", "==", lType, rType);
    }
    else if (token().isInequalityOperator() || token().isAltInequalityOperator()) {
        if ( lType == TypeDescriptor::INTEGER && rType == TypeDescriptor::INTEGER )
            return new IntegerDescriptor(dynamic_cast<IntegerDescriptor*>(lValue)->value() !=
                                         dynamic_cast<IntegerDescriptor*>(rValue)->value());
        else if ( lType == TypeDescriptor::INTEGER && rType == TypeDescriptor::DOUBLE )
            return new IntegerDescriptor(dynamic_cast<IntegerDescriptor*>(lValue)->value() !=
                                         dynamic_cast<DoubleDescriptor*>(rValue)->value());
        else if ( lType == TypeDescriptor::DOUBLE && rType == TypeDescriptor::INTEGER )
            return new IntegerDescriptor(dynamic_cast<DoubleDescriptor*>(lValue)->value() !=
                                         dynamic_cast<IntegerDescriptor*>(rValue)->value());
        else if ( lType == TypeDescriptor::DOUBLE && rType == TypeDescriptor::DOUBLE )
            return new IntegerDescriptor(dynamic_cast<DoubleDescriptor*>(lValue)->value() !=
                                         dynamic_cast<DoubleDescriptor*>(rValue)->value());
        else if ( lType == TypeDescriptor::STRING && rType == TypeDescriptor::STRING )
            return new IntegerDescriptor(dynamic_cast<StringDescriptor*>(lValue)->value() !=
                                         dynamic_cast<StringDescriptor*>(rValue)->value());
        // If operand types are unsupported
        die("RelExprNode::evaluate", "!=", lType, rType);
    }
    else {
        std::cout << "RelExprNode::evaluate: don't know how to evaluate this operator\n";
        token().print();
        std::cout << std::endl;
        exit(2);
    }

    return nullptr; // Will never reach here
}

void RelExprNode::print() {
    _left->print();
    token().print();
    _right->print();
}

// BoolExprNode
BoolExprNode::BoolExprNode(Token tk) : ExprNode{tk}, _left(nullptr), _right(nullptr) {}

ExprNode *&BoolExprNode::left() { return _left; }

ExprNode *&BoolExprNode::right() { return _right; }

TypeDescriptor* BoolExprNode::evaluate(SymTab &symTab) {
    // evaluates a boolean expression using a post-order traversal of the expression tree.
    TypeDescriptor* lValue = left()->evaluate(symTab);
    TypeDescriptor::types lType = lValue->type();
    TypeDescriptor* rValue = right()->evaluate(symTab);
    TypeDescriptor::types rType = rValue->type();
	if (verbose) {
		std::cout << "BoolExprNode::evaluate: ";
		lValue->print();
		token().print();
		rValue->print();
		std::cout << std::endl;
	}
    if (token().isOrOperator()) {
        if ( lType == TypeDescriptor::INTEGER && rType == TypeDescriptor::INTEGER )
            return new IntegerDescriptor(dynamic_cast<IntegerDescriptor*>(lValue)->value() ||
                                         dynamic_cast<IntegerDescriptor*>(rValue)->value());
        // If operand types are unsupported
        die("BoolExprNode::evaluate", "or", lType, rType);
    }
    else if (token().isAndOperator()) {
        if ( lType == TypeDescriptor::INTEGER && rType == TypeDescriptor::INTEGER )
            return new IntegerDescriptor(dynamic_cast<IntegerDescriptor*>(lValue)->value() &&
                                         dynamic_cast<IntegerDescriptor*>(rValue)->value());
        // If operand types are unsupported
        die("BoolExprNode::evaluate", "and", lType, rType);
    }
    else {
        std::cout << "BoolExprNode::evaluate: don't know how to evaluate this operator\n";
        token().print();
        std::cout << std::endl;
        exit(2);
    }

    return nullptr; // Will never reach here
}

void BoolExprNode::print() {
    _left->print();
    token().print();
    _right->print();
}

// NotExprNode
NotExprNode::NotExprNode(Token tk) : ExprNode{tk}, _right(nullptr) {}

ExprNode *&NotExprNode::right() { return _right; }

TypeDescriptor* NotExprNode::evaluate(SymTab &symTab) {
    // evaluates a not expression using a post-order traversal of the expression tree.
    TypeDescriptor* rValue = right()->evaluate(symTab);
    TypeDescriptor::types rType = rValue->type();
	if (verbose) {
		std::cout << "NotExprNode::evaluate:";
		token().print();
		rValue->print();
		std::cout << std::endl;
	}
    if (token().isNotOperator()) {
        if ( rType == TypeDescriptor::INTEGER )
            return new IntegerDescriptor(! dynamic_cast<IntegerDescriptor*>(rValue)->value());
        // If operand types are unsupported
        std::cout << "NotExprNode::evaluate Bad operand type for unary operator not: '";
        switch ( rType ) {
            case ( TypeDescriptor::DOUBLE ):
                std::cout << "Double";
                break;
            case ( TypeDescriptor::STRING ):
                std::cout << "String";
                break;
            case ( TypeDescriptor::ARRAY ):
                std::cout << "Array";
                break;
        }
        std::cout << '\'' << std::endl;
        exit(1);
    }
    else {
        std::cout << "NotExprNode::evaluate: don't know how to evaluate this operator\n";
        token().print();
        std::cout << std::endl;
        exit(2);
    }

    return nullptr; // Will never reach here
}

void NotExprNode::print() {
    token().print();
    _right->print();
}

// Subscription
Subscription::Subscription(Token varName, ExprNode* subscript): ExprNode{varName}, _subscript{subscript} {}

void Subscription::print() {
    token().print();
    std::cout << '[';
    _subscript->print();
    std::cout << ']';
}

TypeDescriptor* Subscription::evaluate(SymTab &symTab) {
	if (verbose) {
		std::cout << "Subscription::evaluate: ";
	}
    TypeDescriptor* subVal = subscript()->evaluate(symTab);
    TypeDescriptor::types subType = subVal->type();
	if (verbose) {
		token().print();
		std::cout << '[';
		subVal->print();
		std::cout << ']';
		std::cout << std::endl;
	}
    if (token().isName()) {
        if ( subType == TypeDescriptor::INTEGER ) {
            int index = dynamic_cast<IntegerDescriptor*>(subVal)->value();
            if (index < 0) {
                std::cout << "Subscription::evaluate Index out of bounds, less than 0" << std::endl;
                exit(1);
            }
            std::string varName = token().getName();
            if (symTab.isDefined(varName)) {
                TypeDescriptor *array = symTab.getValueFor(varName);
                if (array->type() == TypeDescriptor::ARRAY) {
                    std::vector<TypeDescriptor*> values = dynamic_cast<ArrayDescriptor*>(array)->value();
                    if (index >= values.size()) {
                        std::cout << "Subscription::evaluate Index out of bounds, larger than array size" << std::endl;
                        exit(1);
                    }
					if (verbose) {
						std::cout << "Subscription::evaluate returning ";
						values[index]->print();
						std::cout << std::endl;
					}
                    return values[index];
                } else {
                    std::cout << "Subscription::evaluate Can't subscript into a";
                    switch ( subType ) {
                        case ( TypeDescriptor::INTEGER ):
                            std::cout << "n Integer";
                            break;
                        case ( TypeDescriptor::DOUBLE ):
                            std::cout << " Double";
                            break;
                        case ( TypeDescriptor::STRING ):
                            std::cout << " String";
                            break;
                    }
                    std::cout << std::endl;
                    exit(1);
                }
            } else {
                std::cout << "Subscription::evaluate Use of undefined variable, " << varName << std::endl;
                exit(1);
            }
        }
        std::cout << "Subscription::evaluate Bad index type for subscription operation: '";
        switch ( subType ) {
            case ( TypeDescriptor::DOUBLE ):
                std::cout << "Double";
                break;
            case ( TypeDescriptor::STRING ):
                std::cout << "String";
                break;
            case ( TypeDescriptor::ARRAY ):
                std::cout << "Array";
                break;
        }
        std::cout << '\'' << std::endl;
        exit(1);

    } else {
        std::cout << "Subscription::evaluate: don't know how to evaluate this operator\n";
        token().print();
        std::cout << std::endl;
        exit(2);
    }
}

ExprNode *&Subscription::subscript() { return _subscript; }

// Call
Call::Call(Token funcName, Functions* funcList, std::vector<ExprNode*> args): ExprNode{funcName}, _funcList{funcList}, _args{args} {}

void Call::print() {
    token().print();
    std::cout << '(';
    if (!_args.empty()) {
        _args[0]->print();
    }
    for (int i = 1; i < _args.size(); i++) {
        std::cout << ", ";
        _args[i]->print();
    }
    std::cout << ')';
}

TypeDescriptor* Call::evaluate(SymTab &symTab) {
    Function* func = _funcList->findFunction(token().getName());
    std::vector<std::string> params = func->args();
    if (params.size() != _args.size()) {
        std::cout << "Call::evaluate Unequal number or arguments and parameters" << std::endl;
        exit(1);
    }

    std::map<std::string, TypeDescriptor*> newSymTab;
    for (int i = 0; i < params.size(); i++) {
        newSymTab[params[i]] = _args[i]->evaluate(symTab);
    }

    symTab.openScope(newSymTab);

    func->body()->evaluate(symTab);

    TypeDescriptor *ret = symTab.getReturnValue();
    if (ret->type() == TypeDescriptor::NOT_SPECIFIED) {
        std::cout << "Call::evaluate Function has no return value so it cannot be used as an ExprNode" << std::endl;
        exit(1);
    }

    symTab.closeScope();

    return ret;
}

std::vector<ExprNode*> &Call::args() { return _args; }

// Len
Len::Len(Token varName): ExprNode{varName} {}

void Len::print() {
    std::cout << "len(";
    token().print();
    std::cout << ")";
}

TypeDescriptor* Len::evaluate(SymTab &symTab) {
    TypeDescriptor* value = symTab.getValueFor(token().getName());
    if (value->type() != TypeDescriptor::ARRAY) {
        std::cout << "Len::evaluate: Len only works when called on an array" << std::endl;
        exit(1);
    }
    int ret = dynamic_cast<ArrayDescriptor*>(value)->numElements();
	if (verbose) {
		std::cout << "Len::evaluate: returning ";
		std::cout << ret << std::endl;
	}
    return new IntegerDescriptor(ret);
}

// WholeNumber
WholeNumber::WholeNumber(Token token): ExprNode{token} {}

void WholeNumber::print() {
    token().print();
}

TypeDescriptor* WholeNumber::evaluate(SymTab &symTab) {
	if (verbose) { std::cout << "WholeNumber::evaluate: returning " << token().getWholeNumber() << std::endl; }
    return new IntegerDescriptor(token().getWholeNumber());
}

// Variable
Variable::Variable(Token token): ExprNode{token} {}

void Variable::print() {
    token().print();
}

TypeDescriptor* Variable::evaluate(SymTab &symTab) {
    std::string name = token().getName();
    if( ! symTab.isDefined(token().getName())) {
        std::cout << "Variable::evaluate Use of undefined variable, " << token().getName() << std::endl;
        exit(1);
    }
    if (verbose) { 
		std::cout << "Variable::evaluate: returning ";
		symTab.getValueFor(token().getName());
	}
    return symTab.getValueFor(token().getName());
}

// Double
Double::Double(Token token): ExprNode{token} {}

void Double::print() {
    token().print();
}

TypeDescriptor* Double::evaluate(SymTab &symTab) {
	if (verbose) { std::cout << "Double::evaluate: returning " << token().getDouble() << std::endl; }
    return new DoubleDescriptor(token().getDouble());
}

// String
String::String(Token token): ExprNode{token} {}

void String::print() {
    token().print();
}

TypeDescriptor* String::evaluate(SymTab &symTab) {
    if (verbose) { std::cout << "String::evaluate: returning " << token().getString() << std::endl; }
    return new StringDescriptor(token().getString());
}
