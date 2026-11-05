#ifndef HASH_TABLE_ENUMERATOR
#define HASH_TABLE_ENUMERATOR

#include "Enumerator.h"
#include "Exceptions.h"
#include "HashTable.h"
#include "OULinkedList.h"
#include "OULinkedListEnumerator.h"

template <typename T>
class HashTableEnumerator : public Enumerator<T>
{
private:
    unsigned long bucket = 0;                               // the current bucket during the enumeration process
    OULinkedListEnumerator<T> chainEnumerator{};			// used to move through the linked list of the current bucket
    const HashTable<T>* hashTable = nullptr;						    // pointer to the HashTable being enumerated
    bool empty = false;
    T result;

public:
    explicit HashTableEnumerator(const HashTable<T>* hashTable);	// constructor needs a pointer to the HashTable to be enumerated
    virtual ~HashTableEnumerator() = default;
    bool hasNext() const override;							// true if there are elements that have not yet been returned via next()
    T next() override;										// throws ExceptionEnumerationBeyondEnd if no next item is available
    T peek() const override;									// throws ExceptionEnumerationBeyondEnd if no next item is available
};

template <typename T>
HashTableEnumerator<T>::HashTableEnumerator(const HashTable<T>* hashTable) {

    if(hashTable == nullptr || hashTable->getSize() == 0) { // hash table is in invalid state to enumerate, so throw exception
        throw ExceptionHashTableAccess{};
    }

    this->hashTable = hashTable;

    while (bucket < hashTable->getBaseCapacity() && hashTable->table[bucket]->getSize() == 0) { // cycle to a non-empty bucket
        bucket++;
    }
    chainEnumerator = hashTable->table[bucket]->enumerator(); // initialize chain enumerator
}

template <typename T>
bool HashTableEnumerator<T>::hasNext() const {

    unsigned long temp = bucket;
    bool result = false;

    if (chainEnumerator.hasNext()) { // have next, return true
        return true;

    }
    else { // we are at an invalid bucket, so cycle to valid value

        while (temp < hashTable->getBaseCapacity() && hashTable->table[temp]->getSize() == 0) {
            temp++;
        }

        if (temp < hashTable->getBaseCapacity()) { // if temp is less than base capacity, we found a valid value
            result = hashTable->table[temp]->enumerator().hasNext();
        }
    }
    return result; // return false if temp is greater
}

template <typename T>
T HashTableEnumerator<T>::next() {

    if (bucket >= hashTable->getBaseCapacity()) { // bucket is too large, throw exception
        throw ExceptionEnumerationBeyondEnd{};
    }

    if (!empty) { // if the bucket is empty, create a new bucket enumerator

        while (bucket < hashTable->getBaseCapacity() && hashTable->table[bucket]->getSize() == 0) {
            bucket++;
        }

        chainEnumerator = hashTable->table[bucket]->enumerator();
    }

    result = chainEnumerator.next();

    if (!chainEnumerator.hasNext()) { // bucket has no more elements, set bucket to empty
        empty = false;
        bucket++;

    } else { // more elements so keep chain enumerator
        empty = true;
    }

    return result;
}

template <typename T>
T HashTableEnumerator<T>::peek() const {

    unsigned long temp = bucket;

    if (chainEnumerator.hasNext()) { // if table has next value, peek()
        return chainEnumerator.peek();

    } else {

        while (temp < hashTable->getBaseCapacity() && hashTable->table[temp]->getSize() == 0) {
            temp++;
        }

        if (temp < hashTable->getBaseCapacity()) { // as with hasnext, we need to make sure temp does not exceed base capacity
            return hashTable->table[temp]->enumerator().peek(); // create new enumerator for temp and call peek on it
        }
        else { // temp is exceed base capacity so throw exception
            throw ExceptionEnumerationBeyondEnd{};
        }
    }
}
#endif // !HASH_TABLE_ENUMERATOR