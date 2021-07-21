#ifndef COMPARATOR_H
#define COMPARATOR_H

#include "Exceptions.h"

template <typename T>
class Comparator {
public:
    virtual int compare(const T& item1, const T& item2) const = 0;
};

#endif
