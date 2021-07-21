#ifndef HASH_TABLE
#define HASH_TABLE

#include "Exceptions.h"
#include "Comparator.h"
#include "OULinkedList.h"
#include "OULinkedListEnumerator.h"
#include "Hasher.h"
#include "NvraRecord.h"
#include <cmath>
#include <string>
#include <iostream>
#include <fstream>

const unsigned int SCHEDULE_SIZE = 25;			// the number of items in the size schedule
const unsigned int SCHEDULE[SCHEDULE_SIZE] = { 1, 2, 5, 11, 23, 53, 107, 223, 449, 907, 1823, 3659, 7309, 14621, 29243, 58511, 117023, 234067, 468157, 936319, 1872667, 3745283, 7490573, 14981147, 29962343 };		// the size schedule (all primes)
const unsigned int DEFAULT_SCHEDULE_INDEX = 3;	// the default position in the size schedule
const unsigned long DEFAULT_BASE_CAPACITY = SCHEDULE[DEFAULT_SCHEDULE_INDEX]; 	// the default size of the array
const float DEFAULT_MAX_LOAD_FACTOR = 0.9f;		// the default load factor used to determine when to increase the table size
const float DEFAULT_MIN_LOAD_FACTOR = 0.4f;		// the default load factor used to determine when to decrease the table size

template <typename T>
class HashTable {
    template <typename>
    friend class HashTableEnumerator;						// necessary to allow the enumerator to access the table's private data
private:
    Comparator<T>* comparator = NULL;						// used to determine item equality
    Hasher<T>* hasher = NULL;								// used to compute hash value
    unsigned long size = 0;									// actual number of items currently in hash table
    float maxLoadFactor = DEFAULT_MAX_LOAD_FACTOR;			// the load factor used to determine when to increase the table size
    float minLoadFactor = DEFAULT_MIN_LOAD_FACTOR;			// the load factor used to determine when to decrease the table size
    unsigned int scheduleIndex = DEFAULT_SCHEDULE_INDEX;	// the index of current location in the size schedule
    unsigned long baseCapacity = DEFAULT_BASE_CAPACITY;		// the size of the array
    unsigned long totalCapacity = baseCapacity;				// the size of the array plus number of elements from chains of more than one link
    OULinkedList<T>** table = NULL;                         // table will be an array of pointers to OULinkedLists of type T

    // method to copy the table
    void copyTable(OULinkedList<T>** &temp, unsigned long oldCapacity) {

        T item; // item to hold the data
        T* itemPointer; // pointer to the item
        size = 0; // size of the table
        unsigned long bucketIndex; // bucket index
        OULinkedListEnumerator<T> d = NULL; // declare a new enumerator object

        for (unsigned long index = 0; index < oldCapacity; ++index) {
            d = table[index]->enumerator(); // create enumerator object for particular bucket
            while (d.hasNext()) { // copy items to table
                item = d.next();
                itemPointer = &item;
                bucketIndex = getBucketNumber(itemPointer);
                temp[bucketIndex]->insert(itemPointer);
                size++;
                if (table[bucketIndex]->getSize() > 1) {
                    totalCapacity++;
                }
            }
        }
        deleteTable(oldCapacity); // delete the old table
        table = temp;
    }

    // method to decrease the capacity of the table
    void decreaseCapacity() {

        scheduleIndex--;
        unsigned long val = baseCapacity;
        baseCapacity = SCHEDULE[scheduleIndex];
        totalCapacity = baseCapacity;

        auto** temp = new (std::nothrow) OULinkedList<T>*[SCHEDULE[scheduleIndex]];

        if (temp == NULL) { // if no memory available, throw exception!
            throw new ExceptionMemoryNotAvailable();
        }

        for (unsigned long index = 0; index < SCHEDULE[scheduleIndex]; ++index) {
            temp[index] = new OULinkedList<T>(comparator); // create the new table
        }

        copyTable(temp, val); // copy the old table to the new table
    }

    // method to increase the capacity of the table
    void increaseCapacity() {
        scheduleIndex++;
        unsigned long val = baseCapacity;
        baseCapacity = SCHEDULE[scheduleIndex];

        auto** temp = new (std::nothrow) OULinkedList<T>*[SCHEDULE[scheduleIndex]];

        if (temp == NULL) {
            throw new ExceptionMemoryNotAvailable();
        }

        for (unsigned long index = 0; index < SCHEDULE[scheduleIndex]; ++index) {
            temp[index] = new OULinkedList<T>(comparator); // create new table
        }

        copyTable(temp, val); // copy the old table to new table
    }

    // delete the old table
    void deleteTable(unsigned long size){
        for (unsigned long index = 0; index < size; ++index) {
            delete table[index];
        }

        delete[] table;
        table = NULL;
    }

    // method to initialize the table
    void hashTableInitialize(unsigned long size) {

        unsigned long index;
        unsigned int tempIndex = 0;

        auto tableSize = static_cast<unsigned long>(ceil(static_cast<float>(size) * (1/maxLoadFactor))); // determine appropriate table size

        for (index = 0; index < SCHEDULE_SIZE; ++index) {
            if (SCHEDULE[index] >= tableSize) { // need a table size that can fit the elements so set to next largest size
                tempIndex = SCHEDULE[index];
                table = new OULinkedList<T>*[tempIndex]; // create the table
                scheduleIndex = (unsigned int) index;
                break;
            }
        }

        for (unsigned long initIndex = 0; initIndex < tempIndex; ++initIndex) { // initialize the table
            table[initIndex] = new OULinkedList<T>(comparator);
        }

        baseCapacity = SCHEDULE[index]; // set base capacity
        totalCapacity = SCHEDULE[index]; // set total capacity
    }
public:
    HashTable(Comparator<T>* comparator, Hasher<T>* hasher);			// creates an empty table of DEFAULT_BASE_CAPACITY
    // if size given, creates empty table with size from schedule of sufficient capacity (considering maxLoadFactor)
    HashTable(Comparator<T>* comparator, Hasher<T>* hasher, unsigned long size, float maxLoadFactor = DEFAULT_MAX_LOAD_FACTOR, float minLoadFactor = DEFAULT_MIN_LOAD_FACTOR);
    virtual ~HashTable();
    void displayHashTable(const long& records, const long& attemptedRecords, const long& validRecords);

    // if an equivalent item is not already present, insert item at proper location and return true
    // if an equivalent item is already present, leave table unchanged and return false
    bool insert(T* item);

    // if an equivalent item is already present, replace item and return true
    // if an equivalent item is not already present, leave table unchanged and return false
    bool replace(T* item);

    // if an equivalent item is already present, remove item and return true
    // if an equivalent item is not already present, leave table unchanged and return false
    bool remove(T* item);

    // if an equivalent item is present, return a copy of the item
    // if an equivalent item is not present, throw a new ExceptionHashTableAccess
    T find(const T* item) const;

    unsigned long getSize() const;						// returns the current number of items in the table
    unsigned long getBaseCapacity() const;				// returns the current base capacity of the table
    unsigned long getTotalCapacity() const;				// returns the current total capacity of the table
    float getLoadFactor() const;						// returns the current load factor of the table
    unsigned long getBucketNumber(const T* item) const;	// returns the bucket number for an item using its hash function mod array size
    void recreateTable(unsigned long newSize, unsigned long oldSize); // method to recreate the table
};

template <typename T>
HashTable<T>::HashTable(Comparator<T>* comparator, Hasher<T>* hasher) {
    this->comparator = comparator;
    this->hasher = hasher;
    table = new OULinkedList<T>*[DEFAULT_BASE_CAPACITY]; // create table of default capacity
    for (unsigned long index = 0; index < DEFAULT_BASE_CAPACITY; ++index) { // initiliaze the linked lists
        table[index] = new OULinkedList<T>(comparator);
    }
}

template <typename T>
HashTable<T>::HashTable(Comparator<T>* comparator, Hasher<T>* hasher, unsigned long size, float maxLoadFactor, float minLoadFactor) {
    this->comparator = comparator;
    this->hasher = hasher;
    this->maxLoadFactor = maxLoadFactor;
    this->minLoadFactor = minLoadFactor;
    hashTableInitialize(size);
}

template <typename T>
HashTable<T>::~HashTable() {

    deleteTable(baseCapacity);
    comparator = NULL;
    baseCapacity = DEFAULT_BASE_CAPACITY; // reset the base and total capacities
    totalCapacity = baseCapacity;
    size = 0;
}

// method to print the table
template <typename T>
void HashTable<T>::displayHashTable(const long& records, const long& attemptedRecords, const long& validRecords) {

    std::string outputFile;
    std::cout << "Enter output file name: ";
    getline(std::cin, outputFile);

    OULinkedListEnumerator<T> bucketEnum = NULL; // enumerator for bucket

    if (outputFile.empty()) { // no file name entered, send to standard out

        T temp;
        T* tempPointer;

        for (unsigned long index = 0; index < baseCapacity; ++index) {
            bucketEnum = table[index]->enumerator(); // initialize the enum
            bool isFirst = true; // is this the first value in the bucket?
            while (bucketEnum.hasNext()) {
                temp = bucketEnum.next();
                tempPointer = &temp;

                if (isFirst) { // first element, print normally
                    std::cout << getBucketNumber(tempPointer) << ": " << temp << std::endl;
                    isFirst = false;
                } else { // print overflow elements
                    std::cout << "OVERFLOW: " << temp << std::endl;
                }
            }
            if (!isFirst) { // need new line
                std::cout << std::endl;
            }
        }
        std::cout << "Base Capacity: " << baseCapacity << "; Total Capacity: " << totalCapacity << "; Load Factor: "
                  << getLoadFactor() << std::endl;

        std::cout << "Data lines read: " << attemptedRecords <<
                  "; Valid NVRA records read: " << validRecords << "; NVRA records in memory: " <<
                  records << std::endl;
    } else {
        std::ofstream fileToWrite;
        fileToWrite.open(outputFile);

        if (fileToWrite.is_open()) { // file is valid file, so open it

            for (unsigned long index = 0; index < baseCapacity; ++index) {

                bucketEnum = table[index]->enumerator();
                bool isFirst = true;

                while (bucketEnum.hasNext()) {
                    T temp = bucketEnum.next();
                    T *tempPointer = &temp;

                    if (isFirst) {
                        fileToWrite << getBucketNumber(tempPointer) << ": " << temp << std::endl;
                        isFirst = false;
                    } else {
                        fileToWrite << "OVERFLOW: " << temp << std::endl;
                    }
                }
                if (!isFirst) {
                    fileToWrite << std::endl;
                }
            }
            fileToWrite << "Base Capacity: " << baseCapacity << "; Total Capacity: " << totalCapacity << "; Load Factor: "
                        << getLoadFactor() << std::endl;
            fileToWrite << "Data lines read: " << attemptedRecords <<
                        "; Valid NVRA records read: " << validRecords << "; NVRA records in memory: " <<
                        records << std::endl;
            fileToWrite.close();
        } else {
            std::cout << "File is not available." << std::endl;
        }
    }
}

// method to recreate the table
template <typename T>
void HashTable<T>::recreateTable(unsigned long newSize, unsigned long oldSize) {

    deleteTable(oldSize);  // delete the old table
    hashTableInitialize(newSize); // create a new one
    size = 0; // reset the size
}

template <typename T>
bool HashTable<T>::insert(T* item) {

    if (getLoadFactor() >= maxLoadFactor) { // determine if resize needed
        increaseCapacity();
    }

    unsigned long index;
    index = getBucketNumber(item);
    bool cond = table[index]->insert(item); // attempt to insert the item

    if (cond) { // inserted, so return true
        if (table[index]->getSize() > 1) {
            totalCapacity++;
        }
        size++;
        return true;
    } else { // not inserted, return false
        return false;
    }
}

template <typename T>
bool HashTable<T>::replace(T *item) {

    unsigned long index;
    index = getBucketNumber(item);
    bool cond = table[index]->replace(item); // attempt to replace

    return cond;
}

template <typename T>
bool HashTable<T>::remove(T *item) {

    if (getLoadFactor() <= minLoadFactor) { // check if needs resize
        decreaseCapacity();
    }

    unsigned long index;
    index = getBucketNumber(item);
    bool cond = table[index]->remove(item); // attempt to remove the element

    if (cond) { // element removed, return true
        size--;
        return true;
    } else {
        return false;
    }
}

template <typename T>
T HashTable<T>::find(const T* item) const {

    unsigned long index;
    index = getBucketNumber(item);
    T result;
    bool found = false;

    if (table[index] == NULL) { // bucket where element should be is empty, throw exception
        throw new ExceptionHashTableAccess();
    }

    OULinkedListEnumerator<T> listEnum = table[index]->enumerator();

    while (listEnum.hasNext()) { // determine if the item is in the bucket
        result = listEnum.next();
        if (comparator->compare(*item, result) == 0) {
            found = true;
            break;
        }
    }

    if (!found) { // element not found, throw an exception
        throw new ExceptionHashTableAccess();
    }
    return result;
}

template <typename T>
unsigned long HashTable<T>::getSize() const {
    return size;
}

template <typename T>
unsigned long HashTable<T>::getBaseCapacity() const {
    return baseCapacity;
}

template <typename T>
unsigned long HashTable<T>::getTotalCapacity() const {
    return totalCapacity;
}

template <typename T>
float HashTable<T>::getLoadFactor() const {
    float result = static_cast<float>(size)/static_cast<float>(totalCapacity);
    return result;
}

template <typename T>
unsigned long HashTable<T>::getBucketNumber(const T* item) const {
    return hasher->hash(*item) % baseCapacity;
}

#endif // !HASH_TABLE

