#ifndef NVRA_ARRAY_H
#define NVRA_ARRAY_H

#include <memory>
#include <new>
#include <utility>
#include "NvraRecord.h"
#include "Exceptions.h"

const int DEFAULT_ARRAY_CAPACITY = 10; // capacity used in no arg constructor

template<class Type>
class TemplatedArray {
private:

    unsigned long capacity = DEFAULT_ARRAY_CAPACITY; // maximum capacity, in records
    unsigned long size = 0; // actual number of items currently in array
    std::unique_ptr<Type[]> data; // owns the array of records

    // private helper method for doubling capacity
    void doubleCapacity() {

        const unsigned long newCapacity = capacity == 0 ? DEFAULT_ARRAY_CAPACITY : capacity * 2;
        std::unique_ptr<Type[]> tempPointer(new (std::nothrow) Type[newCapacity]);

        if (!tempPointer) {
            throw ExceptionMemoryNotAvailable{};
        }

        for (unsigned long index = 0; index < size; ++index) {
            tempPointer[index] = data[index];
        }
        data = std::move(tempPointer);
        capacity = newCapacity;
    }
    // private helper method for halving capacity
    void halveCapacity() {

        const unsigned long newCapacity = capacity / 2;
        std::unique_ptr<Type[]> tempPointer(new (std::nothrow) Type[newCapacity]);

        if (!tempPointer) {
            throw ExceptionMemoryNotAvailable{};
        }


        for (unsigned long tempIndex = 0; tempIndex < newCapacity; ++tempIndex) {
            tempPointer[tempIndex] = data[tempIndex];
        }
        data = std::move(tempPointer);
        capacity = newCapacity;
    }

public:
    using iterator = Type*;
    using const_iterator = const Type*;

    // constructs array with default capacity
    TemplatedArray() {
        data.reset(new Type[capacity]);
    }

    // constructs array with specified capacity
    explicit TemplatedArray(unsigned long capacity) {
        data.reset(new (std::nothrow) Type[capacity]);

        if (data == nullptr) {
            throw ExceptionMemoryNotAvailable{};
        }

        this->capacity = capacity;
    }

    TemplatedArray(const TemplatedArray& other)
        : capacity(other.capacity), size(other.size) {
        if (capacity == 0) {
            return;
        }

        std::unique_ptr<Type[]> copy(new (std::nothrow) Type[capacity]);
        if (!copy) {
            throw ExceptionMemoryNotAvailable{};
        }

        for (unsigned long index = 0; index < size; ++index) {
            copy[index] = other.data[index];
        }
        data = std::move(copy);
    }

    TemplatedArray& operator=(const TemplatedArray& other) {
        if (this != &other) {
            TemplatedArray copy(other);
            swap(copy);
        }
        return *this;
    }

    TemplatedArray(TemplatedArray&& other) noexcept
        : capacity(other.capacity), size(other.size), data(std::move(other.data)) {
        other.capacity = 0;
        other.size = 0;
    }

    TemplatedArray& operator=(TemplatedArray&& other) noexcept {
        if (this != &other) {
            capacity = other.capacity;
            size = other.size;
            data = std::move(other.data);

            other.capacity = 0;
            other.size = 0;
        }
        return *this;
    }

    // frees array space as object is deleted
    virtual ~TemplatedArray() = default;

    void swap(TemplatedArray& other) noexcept {
        using std::swap;
        swap(capacity, other.capacity);
        swap(size, other.size);
        swap(data, other.data);
    }

    // adds record, increments size, doubles capacity as necessary
    void add(const Type* record) {

        if (size == capacity) {
            doubleCapacity();
        }

        data[size] = *record;
        size++;
    }

    // adds record at specified index, increments size, doubles capacity as necessary
    void addAt(const Type* toAdd, unsigned long index) {

        // invalid index; throw exception
        if (index > size) {
            throw ExceptionIndexOutOfRange{};
        }

        if (size == capacity) {
            doubleCapacity();
        }

        std::unique_ptr<Type[]> tempArray(new (std::nothrow) Type[capacity]);

        // not enough memory; throw exception
        if (!tempArray) {
            throw ExceptionMemoryNotAvailable{};
        }


        for (unsigned long sourceIndex = 0; sourceIndex < index; ++sourceIndex) {
            tempArray[sourceIndex] = data[sourceIndex];
        }
        tempArray[index] = *toAdd;
        for (unsigned long sourceIndex = index; sourceIndex < size; ++sourceIndex) {
            tempArray[sourceIndex + 1] = data[sourceIndex];
        }
        data = std::move(tempArray);
        size++;
    }

    // replaces record at specified index
    void replaceAt(const Type* toReplace, unsigned long index) {

        // invalid index; throw exception
        if (index >= size) {
            throw ExceptionIndexOutOfRange{};
        }

        std::unique_ptr<Type[]> tempArray(new (std::nothrow) Type[capacity]);

        // not enough memory; throw exception
        if (!tempArray) {
            throw ExceptionMemoryNotAvailable{};
        }

        // replace at appropriate index
        for (unsigned long replaceIndex = 0; replaceIndex < size; ++replaceIndex) {
            tempArray[replaceIndex] = replaceIndex == index ? *toReplace : data[replaceIndex];
        }
        data = std::move(tempArray);
    }

    void remove() {

        std::unique_ptr<Type[]> tempArray(new (std::nothrow) Type[capacity]);

        if (!tempArray) {
            throw ExceptionMemoryNotAvailable{};
        }

        data = std::move(tempArray);
        size = 0;
    }

    // removes record at specified index, decrements size, halves capacity as necessary
    void removeAt(unsigned long index) {

        // invalid index; throw exception
        if (index >= size) {
            throw ExceptionIndexOutOfRange{};
        }

        std::unique_ptr<Type[]> tempArray(new (std::nothrow) Type[capacity]);

        // not enough memory; throw exception
        if (!tempArray) {
            throw ExceptionMemoryNotAvailable{};
        }


        for (unsigned long sourceIndex = 0; sourceIndex < index; ++sourceIndex) {
            tempArray[sourceIndex] = data[sourceIndex];
        }
        for (unsigned long sourceIndex = index + 1; sourceIndex < size; ++sourceIndex) {
            tempArray[sourceIndex - 1] = data[sourceIndex];
        }
        data = std::move(tempArray);
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
            throw ExceptionIndexOutOfRange{};
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

    iterator begin() noexcept {
        return data.get();
    }

    const_iterator begin() const noexcept {
        return data.get();
    }

    const_iterator cbegin() const noexcept {
        return data.get();
    }

    iterator end() noexcept {
        return size == 0 ? data.get() : data.get() + size;
    }

    const_iterator end() const noexcept {
        return size == 0 ? data.get() : data.get() + size;
    }

    const_iterator cend() const noexcept {
        return size == 0 ? data.get() : data.get() + size;
    }

    // overloaded index operator
    Type operator[](unsigned long index) const {

        // invalid index; throw exception
        if (index >= size) {
            throw ExceptionIndexOutOfRange{};
        }
        return data[index];
    }
};

template<class Type>
void swap(TemplatedArray<Type>& first, TemplatedArray<Type>& second) noexcept {
    first.swap(second);
}

#endif
