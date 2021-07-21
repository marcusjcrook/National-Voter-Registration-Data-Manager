#ifndef NVRA_ARRAY_H
#define NVRA_ARRAY_H

#include "NvraRecord.h"
#include "Exceptions.h"

const int DEFAULT_ARRAY_CAPACITY = 10; // capacity used in no arg constructor

template<class Type>
class TemplatedArray {
private:

    unsigned long capacity = DEFAULT_ARRAY_CAPACITY; // maximum capacity, in records
    unsigned long size = 0; // actual number of items currently in array
    Type* data = NULL; // pointer to array of records

    // private helper method for doubling capacity
    void doubleCapacity() {

        unsigned long tempCapacity = capacity;
        capacity *= 2;
        Type* tempPointer = new (std::nothrow) Type[capacity];

        if (tempPointer == NULL) {
            throw new ExceptionMemoryNotAvailable;
        }

        for (unsigned long index = 0; index < tempCapacity; ++index) {
            tempPointer[index] = data[index];
        }
        delete[] data;
        data = tempPointer;
    }
    // private helper method for halving capacity
    void halveCapacity() {

        capacity /= 2;
        Type *tempPointer = new (std::nothrow) Type[capacity];

        if (tempPointer == NULL) {
            throw new ExceptionMemoryNotAvailable();
        }


        for (unsigned long tempIndex = 0; tempIndex < capacity; ++tempIndex) {
            tempPointer[tempIndex] = data[tempIndex];
        }
        delete[] data;
        data = tempPointer;
    }

public:

    // constructs array with default capacity
    TemplatedArray() {
        data = new Type[capacity];
    }
    // constructs array with specified capacity
    explicit TemplatedArray(unsigned long capacity) {
        data = new (std::nothrow) Type[capacity];

        if (data == NULL) {
            throw new ExceptionMemoryNotAvailable();
        }

        this->capacity = capacity;
    }
    // frees array space as object is deleted
    virtual ~TemplatedArray() {
        delete[] data;
        data = nullptr;
    }
    // adds record, increments size, doubles capacity as necessary
    void add(Type* record) {

        if (size == capacity) {
            doubleCapacity();
        }

        data[size] = *record;
        size++;
    }

    // adds record at specified index, increments size, doubles capacity as necessary
    void addAt(Type* toAdd, unsigned long index) {

        // invalid index; throw exception
        if (index > size) {
            throw new ExceptionIndexOutOfRange();
        }

        if (size == capacity) {
            doubleCapacity();
        }

        Type* tempArray = new (std::nothrow) Type[capacity];

        // not enough memory; throw exception
        if (tempArray == NULL) {
            throw new ExceptionMemoryNotAvailable();
        }


        unsigned long tempArrayIndex = 0;
        unsigned long dataIndex = 0;

        while (tempArrayIndex < capacity) {

            if (tempArrayIndex == index) {
                tempArray[tempArrayIndex] = *toAdd;
            }
            else {
                tempArray[tempArrayIndex] = data[dataIndex];
                ++dataIndex;
            }
            ++tempArrayIndex;
        }
        delete[] data;
        data = tempArray;
        size++;
    }

    // replaces record at specified index
    void replaceAt(Type* toReplace, unsigned long index) {

        // invalid index; throw exception
        if (index >= size) {
            throw new ExceptionIndexOutOfRange();
        }

        Type* tempArray = new (std::nothrow) Type[capacity];

        // not enough memory; throw exception
        if (tempArray == NULL) {
            throw new ExceptionMemoryNotAvailable();
        }

        // replace at appropriate index
        for (unsigned long replaceIndex = 0; replaceIndex < capacity; ++replaceIndex) {
            if (replaceIndex != index) {
                tempArray[replaceIndex] = data[replaceIndex];
            }
            else {
                tempArray[replaceIndex] = *toReplace;
            }
        }
        delete[] data;
        data = tempArray;
    }

    void remove() {

        delete[] data;
        data = new (std::nothrow) Type[capacity];

        if (data == NULL) {
            throw new ExceptionMemoryNotAvailable();
        }

        size = 0;
    }

    // removes record at specified index, decrements size, halves capacity as necessary
    void removeAt(unsigned long index) {

        // invalid index; throw exception
        if (index >= size) {
            throw new ExceptionIndexOutOfRange();
        }

        Type* tempArray = new (std::nothrow) Type[capacity];

        // not enough memory; throw exception
        if (tempArray == NULL) {
            throw new ExceptionMemoryNotAvailable();
        }


        unsigned long dataIndex = 0;
        unsigned long tempArrayIndex = 0;

        while (dataIndex < capacity) {

            if (dataIndex != index) {
                tempArray[tempArrayIndex] = data[dataIndex];
                ++tempArrayIndex;
            }
            ++dataIndex;
        }
        delete[] data;
        data = tempArray;
        size--;

        // decrement size when necessary
        if (size == (capacity / 2)) {

            halveCapacity();
        }
    }

    // returns (copy of) record at index
    Type get(unsigned long index) const {

        // invalid index; throw exception
        if (index >= size) {
            throw new ExceptionIndexOutOfRange();
        }
        return data[index];
    }

    // returns the capacity of the array
    unsigned long getCapacity() const {
        return capacity;
    }

    //returns the size of the array
    unsigned long getSize() const {
        return size;
    }

    // overloaded index operator
    Type operator[](unsigned long index) const {

        // invalid index; throw exception
        if (index >= size) {
            throw new ExceptionIndexOutOfRange();
        }
        return data[index];
    }
};
#endif
