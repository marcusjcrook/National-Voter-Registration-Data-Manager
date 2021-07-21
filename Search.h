#ifndef SEARCH_H
#define SEARCH_H

#include "Exceptions.h"
#include "TemplatedArray.h"
#include "Comparator.h"

// returns FIRST array location matching the given item (based on the comparator)
template <typename T>
long long binarySearch(const T& item, const TemplatedArray<T>& array, const Comparator<T>& comparator) {
    long mid = 0;
    long low = 0;
    long high = array.getSize() - 1;

    while (high >= low) {
        mid = (high + low) / 2;
        T item2 = array[mid];

        if (comparator.compare(item2, item) < 0) {
            low = mid + 1;
        }
        else if (comparator.compare(item2, item) > 0) {
            high = mid - 1;
        }
        else {
            for (unsigned int index = 0; index < array.getSize(); ++index) {
                if ((mid > 0) && (comparator.compare(array[mid - 1], array[mid]) == 0)) {
                    mid--;
                }
                else {
                    break;
                }
            }
            return mid;
        }
    }

    return -(low + 1);
}
#endif
