#ifndef EXPRINTER_TYPEDESCRIPTOR_HPP
#define EXPRINTER_TYPEDESCRIPTOR_HPP

#include <string>
#include <iostream>
#include <vector>


class TypeDescriptor {
public:
    enum types {INTEGER, DOUBLE, STRING, ARRAY, NOT_SPECIFIED};

    TypeDescriptor(): _type{NOT_SPECIFIED} {}
    TypeDescriptor(types type): _type{type} {}

    types &type() { return _type; }

    virtual ~TypeDescriptor() {}

    virtual void print() = 0;
private:
    types _type;
};

class IntegerDescriptor : public TypeDescriptor {
public:
    IntegerDescriptor(int value): TypeDescriptor(INTEGER), _value(value) {}
    int value() { return _value; }
    void print() { std::cout << value(); }
private:
    int _value;
};

class DoubleDescriptor : public TypeDescriptor {
public:
    DoubleDescriptor(double value): TypeDescriptor(DOUBLE), _value(value) {}
    double value() { return _value; }
    void print() { std::cout << value(); }
private:
    double _value;
};

class StringDescriptor : public TypeDescriptor {
public:
    StringDescriptor(std::string value): TypeDescriptor(STRING), _value(value) {}
    std::string value() { return _value; }
    void print() { std::cout << value(); }
private:
    std::string _value;
};

class ArrayDescriptor : public TypeDescriptor {
public:
    ArrayDescriptor();
    ArrayDescriptor(std::vector<TypeDescriptor*> values, types elType);

    std::vector<TypeDescriptor*> value() const;
    std::vector<TypeDescriptor*> &value();
    types &elType();
    int numElements() const;
    int &numElements();
    void print();
private:
    types _elType;
    int _numElements;
    std::vector<TypeDescriptor*> _values;
};

#endif //EXPRINTER_TYPEDESCRIPTOR_HPP
