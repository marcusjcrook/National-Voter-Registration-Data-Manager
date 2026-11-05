#ifndef SEARCH_H
#define SEARCH_H

#include <algorithm>
#include <iterator>
#include "TemplatedArray.h"
#include "Comparator.h"

// Original hand-written binary search, retained for comparison; the ranges-based
// implementation below is the one currently used.
#if 0
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

// Returns the first matching array index, or -(insertion point + 1) if absent.
template <typename T>
long long binarySearch(const T& item, const TemplatedArray<T>& array, const Comparator<T>& comparator) {
    const auto position = std::ranges::lower_bound(
        array,
        item,
        [&comparator](const T& element, const T& value) {
            return comparator.compare(element, value) < 0;
        });
    const auto index = std::distance(array.cbegin(), position);

    if (position != array.cend() && comparator.compare(*position, item) == 0) {
        return static_cast<long long>(index);
    }
    return -(static_cast<long long>(index) + 1);
}

#endif
