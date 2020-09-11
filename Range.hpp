#ifndef EXPRINTER_RANGE_H
#define EXPRINTER_RANGE_H

#include <vector>
#include "ArithExpr.hpp"

class Range {
public:
    Range(int _rangeValue) : initValue{0},
                             rangeValue{_rangeValue}, stepValue{1} {};
    Range(int _initValue, int _rangeValue) : initValue{_initValue},
                                             rangeValue{_rangeValue}, stepValue{1} {};
    Range(int _initValue, int _rangeValue, int _stepValue) : initValue{_initValue},
                                                             rangeValue{_rangeValue}, stepValue{_stepValue} {};
    bool condition(); // should we iterate?
    int next();       // the value to be assigned to the loop counter.

private:
    int initValue, rangeValue, stepValue;
};


#endif //EXPRINTER_RANGE_H
