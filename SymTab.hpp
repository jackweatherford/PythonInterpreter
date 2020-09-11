#ifndef EXPRINTER_SYMTAB_HPP
#define EXPRINTER_SYMTAB_HPP

#include <string>
#include <map>
#include "TypeDescriptor.hpp"

// This is a flat symbol table. It allows for variables to be
// initialized, determines if a give variable has been defined or not, and if
// a variable has been defined, it returns its value.

class SymTab {
public:
    SymTab();

    bool isDefined(std::string vName);

    void setValueFor(std::string vName, TypeDescriptor* td);
    TypeDescriptor* getValueFor(std::string vName);

    void openScope(std::map<std::string, TypeDescriptor*> newSymTab);
    void storeReturnValue(TypeDescriptor *val);
    TypeDescriptor* getReturnValue();
    void closeScope();

    int getI() { return i; }

    void print();

private:
    std::vector<std::map<std::string, TypeDescriptor*> > symTabs;
    int i; // current SymTab depth
    TypeDescriptor* returnVal;

};

#endif //EXPRINTER_SYMTAB_HPP
