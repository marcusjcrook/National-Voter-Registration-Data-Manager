#ifndef SORT_H
#define SORT_H

#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <utility>
#include "Exceptions.h"
#include "TemplatedArray.h"
#include "Comparator.h"

template <typename T>
class Sorter {
private:
    // private helper quicksort method
    static void qsort(TemplatedArray<T>& arr, long leftIndex, long rightIndex, const Comparator<T>& comparator);

public:
    static void sort(TemplatedArray<T>& array, const Comparator<T>& comparator);
};

template <typename T>
void Sorter<T>::qsort(TemplatedArray<T>& arr, long leftIndex, long rightIndex, const Comparator<T>& comparator) {

   long left;
   long right;
   T pivot;

   if (leftIndex >= rightIndex)  {
       return;
   }

   left = leftIndex;
   right = rightIndex;

   // set the pivot to the middle element
   pivot = arr[(leftIndex + rightIndex) / 2];

    while(left <= right) {

        while (comparator.compare(arr[left], pivot) < 0) {
            left++;
        }
        while (comparator.compare(arr[right], pivot) > 0) {
            right--;
        }
        if (left <= right) {

            // needed left and right to be signed, but needed them to be unsigned when swapping
            unsigned long unsignedLeft = unsigned(left);
            unsigned long unsignedRight = unsigned(right);

            // to avoid exception of passing temporary pointer
            T leftItem = arr[unsignedLeft];
            T rightItem = arr[unsignedRight];

            T &leftValue = leftItem;
            T &rightValue = rightItem;

            arr.replaceAt(&leftValue, unsignedRight);
            arr.replaceAt(&rightValue, unsignedLeft);

            left++;
            right--;
        }
    }

    // recursive call to qsort
    qsort(arr,leftIndex,right, comparator);
    qsort(arr,left,rightIndex, comparator);
}


template <typename T>
void Sorter<T>::sort(TemplatedArray<T>& array, const Comparator<T>& comparator) {
    // call private helper quick sort method
    qsort(array, 0, array.getSize()-1, comparator);
}

#endif
