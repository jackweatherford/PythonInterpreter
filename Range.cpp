#include "Range.hpp"

bool Range::condition() {
    return (initValue < rangeValue);
}

int Range::next() {
    int oldValue = initValue;
    initValue += stepValue;
    return oldValue;
}
