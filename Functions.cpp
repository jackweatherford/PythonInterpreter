#include "Functions.hpp"

void Functions::addFunction(Function *func) {
    functions().push_back(func);
}

Function* Functions::findFunction(std::string name) {
    for (int i = 0; i < _functions.size(); i++) {
        if (_functions[i]->name() == name)
            return _functions[i];
    }
    return nullptr;
}
