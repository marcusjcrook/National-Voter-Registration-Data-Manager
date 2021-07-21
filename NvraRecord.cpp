#include "NvraRecord.h"
#include <string>
#include <utility>

using namespace std;

// overloaded operator to allow printing of records
ostream& operator<<(std::ostream& os, const NvraRecord& record) {
    int loopIndex = 0;
    int numIndex = 0;

    while(loopIndex < 24) {

        switch(loopIndex) {
            case 3:
                os << record.strings[0] << ";";
                break;
            case 11:
                os << record.strings[1] << ";";
                break;
            case 12:
                os << record.strings[2] << ";";
                break;
            default:
                if (numIndex < 21) {
                    os << record.nums[numIndex];

                    // place semicolons in the appropriate location
                    if (numIndex < 20) {
                        os << ";";
                    }
                    numIndex++;
                }
        }
        loopIndex++;
    }
    return os;
}

// add the number to the array
void NvraRecord::addNum(unsigned int num) {
    nums[numCtr] = num;
    numCtr++;
}

// add the string to the array
void NvraRecord::addString(std::string string) {
    strings[strCtr] = std::move(string);
    strCtr++;
}

// get the number at the specified index
unsigned int NvraRecord::getNum(unsigned int index) const {
    return nums[index];
}

// get the string at the specified index
string NvraRecord::getString(unsigned int index) const {
    return strings[index];
}

// error coding here
void NvraRecord::setNum(unsigned int num, unsigned int index) {
    nums[index] = num;
}

// set the string to the value at the index specified
void NvraRecord::setString(std::string string, unsigned int index) {
    strings[index] = std::move(string);
}