#include "NvraComparator.h"
#include <string>

NvraComparator::NvraComparator(unsigned int column) {
    this->column = column;
}

int NvraComparator::compare(const NvraRecord& item1, const NvraRecord& item2) const {

    unsigned int index;
    // if column is 3, 11, or 12, then compare by strings.
    if (column == 3) {
        return item1.getString(0).compare(item2.getString(0));
    }
    else if (column == 11) {
        return item1.getString(1).compare(item2.getString(1));
    }
    else if (column == 12) {
        return item1.getString(2).compare(item2.getString(2));
    }
    // column not string column so compare by numbers
    else {
        // set index to correct column
        if (column > 3 && column < 11) {
            index = column - 1; // did this to offset the index
        }
        else if (column > 12) {
            index = column - 3; // had to offset the index here since we have strings and ints
        }
        else {
            index = column;
        }

        // determine if greater, less, or equal, return appropriate value
        if (item1.getNum(index) > item2.getNum(index)) {
            return 1;
        }
        else if (item1.getNum(index) < item2.getNum(index)) {
            return -1;
        }
        else {
            return 0;
        }
    }
}

