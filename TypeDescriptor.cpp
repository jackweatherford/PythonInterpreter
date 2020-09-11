#include "TypeDescriptor.hpp"

ArrayDescriptor::ArrayDescriptor(): TypeDescriptor(ARRAY), _elType(NOT_SPECIFIED), _numElements(0), _values(std::vector<TypeDescriptor*>()) {}
ArrayDescriptor::ArrayDescriptor(std::vector<TypeDescriptor*> values, types elType): TypeDescriptor(ARRAY), _elType(elType),
                                                        _numElements(values.size()), _values(values) {}

std::vector<TypeDescriptor*> ArrayDescriptor::value() const {
    return _values;
}

std::vector<TypeDescriptor*> &ArrayDescriptor::value() {
    return _values;
}

TypeDescriptor::types &ArrayDescriptor::elType() {
    return _elType;
}

int ArrayDescriptor::numElements() const{
    return _numElements;
}

int &ArrayDescriptor::numElements() {
    return _numElements;
}

void ArrayDescriptor::print() {
    if (numElements() == 0) {
        std::cout << "[]";
    } else if (elType() == INTEGER) {
        std::cout << '[' << dynamic_cast<IntegerDescriptor*>(_values[0])->value();
        for (int i = 1; i < numElements(); i++) {
            std::cout << ", " << dynamic_cast<IntegerDescriptor*>(value()[i])->value();
        }
        std::cout << ']';
    } else if (elType() == DOUBLE) {
        std::cout << '[' << dynamic_cast<DoubleDescriptor*>(_values[0])->value();
        for (int i = 1; i < numElements(); i++) {
            std::cout << ", " << dynamic_cast<DoubleDescriptor*>(value()[i])->value();
        }
        std::cout << ']';
    } else if (elType() == STRING) {
        std::cout << '[' << dynamic_cast<StringDescriptor*>(_values[0])->value();
        for (int i = 1; i < numElements(); i++) {
            std::cout << ", " << dynamic_cast<StringDescriptor*>(_values[i])->value();
        }
        std::cout << ']';
    } else {
        std::cout << "ArrayDescriptor::print Element type is Array or Unspecified";
        exit(1);
    }
}
