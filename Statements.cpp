#include "Statements.hpp"

// Statement
Statement::Statement() {}

// Statements

Statements::Statements() {}
void Statements::addStatement(Statement *statement) { _statements.push_back(statement); }

void Statements::print() {
    for (auto s: _statements)
        s->print();
}

void Statements::evaluate(SymTab &symTab) {
    for (auto s: _statements) {
        s->evaluate(symTab);
        if (dynamic_cast<ReturnStatement*>(s))
            return;
    }
}

// AssignmentStatement

AssignmentStatement::AssignmentStatement() : _lhsExpression{nullptr}, _rhsExpression{nullptr} {}

AssignmentStatement::AssignmentStatement(ExprNode *lhsExpr, ExprNode *rhsExpr):
        _lhsExpression{lhsExpr}, _rhsExpression{rhsExpr} {}

AssignmentStatement::AssignmentStatement(ExprNode *lhsExpr, std::vector<ExprNode *> rhsArray):
        _lhsExpression(lhsExpr), _rhsExpression{nullptr}, _rhsArray{rhsArray} {}

void AssignmentStatement::evaluate(SymTab &symTab) {
    if (_rhsExpression == nullptr) {
        if (dynamic_cast<Variable*>(lhsExpression()) == nullptr) {
            std::cout << "AssignmentStatement::evaluate Can't assign arrays to Subscripts (Multidimensional arrays are unsupported)\n";
            exit(1);
        }
        Variable* lhs = dynamic_cast<Variable*>(lhsExpression());
        if (_rhsArray.empty()) {
            symTab.setValueFor(lhs->token().getName(), new ArrayDescriptor());
            return;
        }
        std::vector<TypeDescriptor*> arr;
        TypeDescriptor* val = _rhsArray[0]->evaluate(symTab);
        TypeDescriptor::types arrType = val->type();
        if (arrType == TypeDescriptor::ARRAY || arrType == TypeDescriptor::NOT_SPECIFIED) {
            std::cout << "AssignmentStatement::evaluate Right hand side array elements are of an unsupported type\n";
            exit(1);
        }
        arr.push_back(val);
        for (int i = 1; i < _rhsArray.size(); i++) {
            val = _rhsArray[i]->evaluate(symTab);
            if (val->type() != arrType) {
                std::cout << "AssignmentStatement::evaluate Data types of all elements in the right hand side array must be the same\n";
                exit(1);
            }
            arr.push_back(val);
        }
        ArrayDescriptor* arrDesc = new ArrayDescriptor(arr, arrType);
        symTab.setValueFor(lhs->token().getName(), arrDesc);
        return;
    }
    TypeDescriptor* rhs = rhsExpression()->evaluate(symTab);
    if (dynamic_cast<Variable*>(lhsExpression()) != nullptr) {
        Variable* lhs = dynamic_cast<Variable*>(lhsExpression());
        symTab.setValueFor(lhs->token().getName(), rhs);
    }
    else if (dynamic_cast<Subscription*>(lhsExpression()) != nullptr) {
        Subscription* lhs = dynamic_cast<Subscription*>(lhsExpression());

        TypeDescriptor* subVal = lhs->subscript()->evaluate(symTab);
        if (subVal->type() != TypeDescriptor::INTEGER) {
            std::cout << "AssignmentStatement::evaluate Left hand side Subscription is not an Integer\n";
            exit(1);
        }
        int index = dynamic_cast<IntegerDescriptor*>(subVal)->value();
        if (index < 0) {
            std::cout << "AssignmentStatement::evaluate Left hand side Subscription index out of bounds, less than 0" << std::endl;
            exit(1);
        }
        std::string varName = lhs->token().getName();
        if (!symTab.isDefined(varName)) {
            std::cout << "AssignmentStatement::evaluate Use of undefined variable, " << varName << std::endl;
            exit(1);
        }
        TypeDescriptor *array = symTab.getValueFor(varName);
        if (array->type() != TypeDescriptor::ARRAY) {
            std::cout << "AssignmentStatement::evaluate Left hand side variable is not subscriptable\n";
            exit(1);
        }
        ArrayDescriptor* arrDesc = dynamic_cast<ArrayDescriptor *>(array);
        std::vector<TypeDescriptor *> values = arrDesc->value();
        if (index >= values.size()) {
            std::cout << "AssignmentStatement::evaluate Left hand side index out of bounds, larger than array size" << std::endl;
            exit(1);
        }
        if (rhs->type() != arrDesc->elType()) {
            std::cout << "AssignmentStatement::evaluate Left hand side array's element type does not match right hand side type" << std::endl;
            exit(1);
        }
        arrDesc->value()[index] = rhs;
    } else {
        std::cout << "AssignmentStatement::evaluate Left hand side expression must be either a Variable or Subscription\n";
        exit(1);
    }
}

ExprNode *&AssignmentStatement::lhsExpression() {
    return _lhsExpression;
}

ExprNode *&AssignmentStatement::rhsExpression() {
    return _rhsExpression;
}

std::vector<ExprNode*> AssignmentStatement::rhsArray() {
    return _rhsArray;
}

void AssignmentStatement::print() {
    _lhsExpression->print();
    std::cout << " = ";
    if (_rhsExpression == nullptr) {
        if (_rhsArray.empty()) {
            std::cout << "[]" << std::endl;
            return;
        }
        std::cout << '[';
        _rhsArray[0]->print();
        for (int i = 1; i < _rhsArray.size(); i++) {
            std::cout << ", ";
            _rhsArray[i]->print();
        }
        std::cout << ']';
    } else {
        _rhsExpression->print();
    }
    std::cout << std::endl;
}

// PrintStatement

PrintStatement::PrintStatement() : _list{nullptr} {}

PrintStatement::PrintStatement(std::vector<ExprNode*> list):
        _list{list} {}

void PrintStatement::evaluate(SymTab &symTab) {
    for (int i = 0; i < _list.size(); i++) {
        _list[i]->evaluate(symTab)->print();
        if (i < _list.size() - 1)
            std::cout << " ";
    }
    std::cout << std::endl;
}

std::vector<ExprNode*> &PrintStatement::list() {
    return _list;
}

void PrintStatement::print() {
    std::cout << "print ";
    for (int i = 0; i < _list.size(); i++) {
        _list[i]->print();
        if (i < _list.size() - 1)
            std::cout << ", ";
    }
    std::cout << std::endl;
}

// ForStatement

ForStatement::ForStatement() : _loopVar{Token()}, _rangeArgs{nullptr}, _body{nullptr} {}

ForStatement::ForStatement(Token loopVar, std::vector<ExprNode*> rangeArgs, Statements *body):
        _loopVar{loopVar}, _rangeArgs{rangeArgs}, _body{body} {}

void ForStatement::evaluate(SymTab &symTab) {
    TypeDescriptor* init;
    TypeDescriptor* end;
    TypeDescriptor* step;
    TypeDescriptor::types type;
    std::vector<ExprNode*> args = rangeArgs();
    Range* range;

    if (args.size() == 1) {
        end = args[0]->evaluate(symTab);
        type = end->type();
        if (type != TypeDescriptor::INTEGER) {
            std::cout << "ForStatement::evaluate The first argument in range must be an integer\n";
            exit(1);
        }
        range = new Range(dynamic_cast<IntegerDescriptor*>(end)->value());
    } else if (args.size() == 2) {
        init = args[0]->evaluate(symTab);
        type = init->type();
        if (type != TypeDescriptor::INTEGER) {
            std::cout << "ForStatement::evaluate The first argument in range must be an integer\n";
            exit(1);
        }
        end = args[1]->evaluate(symTab);
        type = end->type();
        if (type != TypeDescriptor::INTEGER) {
            std::cout << "ForStatement::evaluate The second argument in range must be an integer\n";
            exit(1);
        }
        range = new Range(dynamic_cast<IntegerDescriptor*>(init)->value(),
                dynamic_cast<IntegerDescriptor*>(end)->value());
    } else {
        init = args[0]->evaluate(symTab);
        type = init->type();
        if (type != TypeDescriptor::INTEGER) {
            std::cout << "ForStatement::evaluate The first argument in range must be an integer\n";
            exit(1);
        }
        end = args[1]->evaluate(symTab);
        type = end->type();
        if (type != TypeDescriptor::INTEGER) {
            std::cout << "ForStatement::evaluate The second argument in range must be an integer\n";
            exit(1);
        }
        step = args[2]->evaluate(symTab);
        type = step->type();
        if (type != TypeDescriptor::INTEGER) {
            std::cout << "ForStatement::evaluate The third argument in range must be an integer\n";
            exit(1);
        }
        range = new Range(dynamic_cast<IntegerDescriptor*>(init)->value(),
                dynamic_cast<IntegerDescriptor*>(end)->value(),
                dynamic_cast<IntegerDescriptor*>(step)->value());
    }
    Token val;
    std::string varName = loopVar().getName();
    while( range->condition() ) {
        val.setWholeNumber(range->next());
        symTab.setValueFor(varName, WholeNumber(val).evaluate(symTab));
        body()->evaluate(symTab);
    }
}

Token &ForStatement::loopVar() {
    return _loopVar;
}

std::vector<ExprNode*> &ForStatement::rangeArgs() {
    return _rangeArgs;
}

Statements *&ForStatement::body() {
    return _body;
}

void ForStatement::print() {
    std::cout << "for ";
    _loopVar.print();
    std::cout << " in range(";
    _rangeArgs[0]->print();
    for (int i = 1; i < _rangeArgs.size(); i++) {
        std::cout << ", ";
        _rangeArgs[i]->print();
    }
    std::cout << "):" << std::endl;
    _body->print();
}

// IfStatement

IfStatement::IfStatement() : _conditions{nullptr}, _bodies{nullptr} {}

IfStatement::IfStatement(std::vector<ExprNode*> conditions, std::vector<Statements*> bodies): _conditions{conditions},
                            _bodies{bodies} {}

void IfStatement::evaluate(SymTab &symTab) {
    bool done = false;
    int i;
    int size = conditions().size();
    for (i = 0; i < size; i++) {
        TypeDescriptor *cond = conditions()[i]->evaluate(symTab);
        if (cond->type() != TypeDescriptor::INTEGER) {
            std::cout << "IfStatement::evaluate The condition must evaluate to an integer\n";
            exit(1);
        }
        int val = dynamic_cast<IntegerDescriptor *>(cond)->value();
        if (val == 1) {
            bodies()[i]->evaluate(symTab);
            done = true;
            break;
        } else if (val == 0) {
            continue;
        } else {
            std::cout << "IfStatement::evaluate The condition must evaluate to either 0 or 1\n";
            exit(1);
        }
    }
    if (!done && bodies().size() > size) { // if there's an else statement
        bodies()[i]->evaluate(symTab);
    }
}

std::vector<ExprNode*> &IfStatement::conditions() {
    return _conditions;
}

std::vector<Statements*> &IfStatement::bodies() {
    return _bodies;
}

void IfStatement::print() {
    std::cout << "if ";
    conditions()[0]->print();
    std::cout << ":" << std::endl;
    bodies()[0]->print();
    int i;
    int size = conditions().size();
    for (i = 1; i < size; i++) {
        std::cout << "elif ";
        conditions()[i]->print();
        std::cout << ":" << std::endl;
        bodies()[i]->print();
    }
    if (bodies().size() > size) { // if there's an else statement
        std::cout << "else:" << std::endl;
        bodies()[i]->print();
    }
}

// ArrayOpStatement

ArrayOpStatement::ArrayOpStatement() : _arg{nullptr}, _varName{Token()} {}

ArrayOpStatement::ArrayOpStatement(Token varName, ExprNode* arg): _arg{arg}, _varName{varName} {}

void ArrayOpStatement::evaluate(SymTab &symTab) {
    TypeDescriptor* value = symTab.getValueFor(varName().getName());
    if (!symTab.isDefined(varName().getName())) {
        std::cout << "ArrayOpStatement::evaluate Use of undefined variable, " << varName().getName() << std::endl;
        exit(1);
    }
    if (arg() == nullptr) {
        if (value->type() != TypeDescriptor::ARRAY) {
            std::cout << "ArrayPopStatement::evaluate TypeError: Can only pop from an array" << std::endl;
            exit(1);
        }
        ArrayDescriptor* arr = dynamic_cast<ArrayDescriptor*>(value);
        int numElems = arr->numElements();
        if (numElems == 0) {
            std::cout << "ArrayPopStatement::evaluate Array is empty, no elements to pop" << std::endl;
            exit(1);
        }
        arr->value().pop_back();
        arr->numElements() = numElems - 1;
    } else {
        if (value->type() != TypeDescriptor::ARRAY) {
            std::cout << "ArrayPushStatement::evaluate TypeError: Can only push onto an array" << std::endl;
            exit(1);
        }
        ArrayDescriptor* arr = dynamic_cast<ArrayDescriptor*>(value);
        TypeDescriptor* elem = arg()->evaluate(symTab);
        if (arr->numElements() == 0) {
            arr->elType() = elem->type();
        } else if (arr->elType() != elem->type()) {
            std::cout << "ArrayPushStatement::evaluate TypeError: Can only push elements of the same type onto this array" << std::endl;
            exit(1);
        }
        arr->value().push_back(elem);
        arr->numElements() = arr->numElements() + 1;
    }
}

Token &ArrayOpStatement::varName() {
    return _varName;
}

ExprNode* &ArrayOpStatement::arg() {
    return _arg;
}

void ArrayOpStatement::print() {
    std::cout << varName().getName() << '.';
    if (arg() == nullptr) {
        std::cout << "pop()" << std::endl;
    } else {
        std::cout << "push(";
        arg()->print();
        std::cout << ")" << std::endl;
    }
}

// CallStatement

CallStatement::CallStatement(Token funcName, std::vector<std::string> params, std::vector<ExprNode*> args, Statements* body)
    : _funcName{funcName}, _params{params}, _args{args}, _body{body} {}

void CallStatement::evaluate(SymTab &symTab) {
    if (_params.size() != _args.size()) {
        std::cout << "CallStatement::evaluate Unequal number or arguments and parameters" << std::endl;
        exit(1);
    }

    std::map<std::string, TypeDescriptor*> newSymTab;
    for (int i = 0; i < _params.size(); i++) {
        newSymTab[_params[i]] = _args[i]->evaluate(symTab);
    }

    symTab.openScope(newSymTab);

    _body->evaluate(symTab);

    TypeDescriptor *ret = symTab.getReturnValue(); // it returns, but return value goes nowhere

    symTab.closeScope();
}

std::vector<std::string> &CallStatement::params() {
    return _params;
}

std::vector<ExprNode*> &CallStatement::args() {
    return _args;
}

Statements* &CallStatement::body() {
    return _body;
}

void CallStatement::print() {
    std::cout << _funcName.getName() << '(';
    if (!_args.empty()) {
        _args[0]->print();
    }
    for (int i = 1; i < _args.size(); i++) {
        std::cout << ", ";
        _args[i]->print();
    }
    std::cout << ')' << std::endl;
}

// ReturnStatement

ReturnStatement::ReturnStatement(ExprNode *retVal) : _retVal{retVal} {}

void ReturnStatement::evaluate(SymTab &symTab) {
    if (symTab.getI() == 0) {
        std::cout << "ReturnStatement::evaluate Can't return from global scope" << std::endl;
        exit(1);
    }
    symTab.storeReturnValue(_retVal->evaluate(symTab));
}

ExprNode* &ReturnStatement::retVal() {
    return _retVal;
}

void ReturnStatement::print() {
    std::cout << "return ";
    _retVal->print();
    std::cout << std::endl;
}
