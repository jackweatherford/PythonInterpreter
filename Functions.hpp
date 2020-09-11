#ifndef EXPRINTER_FUNCTIONS_HPP
#define EXPRINTER_FUNCTIONS_HPP

#include <vector>
#include "SymTab.hpp"
#include "Statements.hpp"

class Function {
public:
    Function();
    Function(std::string name, std::vector<std::string> args, Statements *body)
            : _name{name}, _args{args}, _body{body} {}

    std::string &name() { return _name; }
    std::vector<std::string> &args() { return _args; }
    Statements *&body() {return _body; }

private:
    std::string _name;
    std::vector<std::string> _args;
    Statements *_body;
};


// Functions is a collection of Function.

class Functions {
public:
    Functions() : _functions{std::vector<Function *>()} {}

    std::vector<Function*> &functions() {return _functions; }

    void addFunction(Function *func);
    Function *findFunction(std::string name);

private:
    std::vector<Function *> _functions;
};


#endif //EXPRINTER_FUNCTIONS_HPP
