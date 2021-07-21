#ifndef OU_LINKED_LIST
#define OU_LINKED_LIST

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include "OULink.h"
#include "Comparator.h"
#include "OULinkedListEnumerator.h"

// OULinkedList stands for Ordered, Unique Linked List. It is a linked list that is always maintained in
// order (based on the comparator provided to it when the list is created) and that only contains unique
// items (that is, duplicates are not allowed)
template <typename T>
class OULinkedList {
    template <typename>
    friend class OULinkedListEnumerator;

private:
    Comparator<T>* comparator = NULL;				// used to determine list order and item equality
    unsigned long size = 0;							// actual number of items currently in list
    OULink<T>* first = NULL;						// pointer to first link in list
    OULink<T>* last = NULL;							// pointer to last link in list
    bool DuplicateCheck(T* item);                   // private helper method to check for duplictates
public:
    explicit OULinkedList(Comparator<T>* comparator);		// creates empty linked list with comparator
    virtual ~OULinkedList();						// deletes all links and their data items

    // method to print contents of LinkedList
    void PrintLinkedList(const long& records, const long& attemptedRecords, const long& validRecords);

    // if an equivalent item is not already present, insert item in order and return true
    // if an equivalent item is already present, leave list unchanged and return false
    bool insert(const T* item);

    // if item is greater than item at last, append item at end and return true
    // if item is less than or equal to item at last, leave list unchanged and return false
    bool append(const T* item);

    // if an equivalent item is already present, replace item and return true
    // if an equivalent item is not already present, leave list unchanged and return false
    bool replace(const T* item);
    //linear search

    // if an equivalent item is already present, remove item and return true
    // if an equivalent item is not already present, leave list unchanged and return false
    bool remove(const T* item);

    // if any items are present, return a copy of the first item
    // if no items are present, throw new ExceptionLinkedListAccess
    T get() const;

    OULink<T>* getFirstLink();

    // if an equivalent item is present, return a copy of the first such item
    // if an equivalent item is not present, throw a new ExceptionLinkedListAccess
    T find(const T* item) const;
    // linear search

    // returns the current number of items in the list
    unsigned long getSize() const;

    // create an enumerator for this linked list
    OULinkedListEnumerator<T> enumerator() const;
};
template <typename T>
void OULinkedList<T>::PrintLinkedList(const long& records, const long& attemptedRecords, const long& validRecords) {

    std::string outputFile;
    std::cout << "Enter output file name: ";
    getline(std::cin, outputFile);

    if (outputFile.empty()) {
        auto current = first;
        unsigned int index = 0;
        while (index < size) {
            std::cout << *current->data << std::endl;
            current = current->next;
            ++index;
        }
    }
    else {
        std::ofstream fileToWrite;
        fileToWrite.open(outputFile);

        if (fileToWrite.is_open()) {
            auto current = first;
            unsigned int index = 0;
            while (index < size) {
                fileToWrite << *current->data << std::endl;
                current = current->next;
                ++index;
            }
            fileToWrite << "Data lines read: " << attemptedRecords <<
                        "; Valid NVRA records read: " << validRecords << "; NVRA records in memory: " <<
                        records << std::endl;
            fileToWrite.close();
        }
        else {
            std::cout << "File is not available." << std::endl;
        }
    }
}

template <typename T>
OULinkedList<T>::OULinkedList(Comparator<T>* comparator) {
    this->comparator = comparator;
}

template <typename T>
OULinkedList<T>::~OULinkedList() {
    this->comparator = NULL;
    if (first != NULL) {
        delete first;
        first = NULL;
    }
}

template <typename T>
bool OULinkedList<T>::DuplicateCheck(T* item) {
    OULink<T>* current = first;
    unsigned int index = 0;

    while (index < size) {
        if (comparator->compare(*current->data, *item) == 0) {
            return true;
        }
        current = current->next;
        index++;
    }
    return false;
}

template <typename T>
bool OULinkedList<T>::insert(const T* item) {

    T* data = new (std::nothrow) T(*item);
    bool isInserted = false;
    auto* listItem = new (std::nothrow) OULink<T>(data);

    if (data == NULL || listItem == NULL) {
        throw new ExceptionMemoryNotAvailable();
    }

    // insert at beginning
    if (first == NULL) {

        first = listItem;
        last = first;
        size++;
        return true;
    }
    // insert at beginning and set next
    else if (comparator->compare(*data, *first->data) < 0) {
        listItem->next = first;
        first = listItem;
        //last = first;
        size++;
        return true;
    }
    else {
        // insert somewhere in middle
        bool duplicate = DuplicateCheck(data);
        if (duplicate) {
            return false;
        }

        OULink<T>* currentItem = first;
        OULink<T>* previousItem = first;

        while (currentItem != NULL) {

            if (comparator->compare(*currentItem->data, *data) > 0) {
                previousItem->next = listItem;
                listItem->next = currentItem;
                size++;
                isInserted = true;
                break;
            }

            previousItem = currentItem;
            currentItem = currentItem->next;
        }

        // insert at end
        if (currentItem == NULL) {

            last->next = listItem;
            last = last->next;
            size++;
            isInserted = true;
        }
    }

    return isInserted;
}

template<typename T>
bool OULinkedList<T>::append(const T* item) {

    auto* listItem = new (std::nothrow) OULink<T>(item);

    if (listItem == NULL) {
        throw new ExceptionMemoryNotAvailable();
    }

    // if no first item, set the first item with above item
    if (first == NULL) {
        first = listItem;
        last = first;
        size++;
        return true;
    }
    // check if item is greater than the data held by last
    else if (comparator->compare(*item, *last->data) > 0) {
        last->next = listItem;
        last = last->next;
        size++;
        return true;
    }
    else {
        return false;
    }
}

template<typename T>
bool OULinkedList<T>::replace(const T* item) {

    auto* replaceItem = new (std::nothrow) T(*item);

    if (replaceItem == NULL) {
        throw new ExceptionMemoryNotAvailable();
    }

    // list is empty so return false
    if (first == NULL) {
        return false;
    }
    // first item is identical to item argument passed in
    else if (comparator->compare(*item, *first->data) == 0) {
        *first->data = *replaceItem;
        return true;
    }
    else {

        // check other values to replace
        OULink<T>* currentItem = first;

        while (currentItem != NULL) {

            if (comparator->compare(*currentItem->data, *item) == 0) {
                *currentItem->data = *replaceItem;

                return true;
            }
            currentItem = currentItem->next;
        }
        return false;
    }
}

template<typename T>
bool OULinkedList<T>::remove(const T* item) {

    auto* listItem = new (std::nothrow) OULink<T>(item);

    if (listItem == NULL) {
        throw new ExceptionMemoryNotAvailable();
    }

    // list is empty so nothing to remove
    if (first == NULL) {
        return false;
    }
    // item is identical with data held by first, so delete first
    else if (comparator->compare(*item, *first->data) == 0) {
        first = first->next;
        size--;
        return true;
    }
    else {

        // remove somewhere else in list
        OULink<T>* currentItem = first;
        OULink<T>* previousItem = first;

        while (currentItem != NULL) {

            if (comparator->compare(*currentItem->data, *listItem->data) == 0) {
                    previousItem->next = currentItem->next;

                if (comparator->compare(*currentItem->data, *last->data) == 0) {
                    last = previousItem;
                }

                size--;
                return true;
            }
            previousItem = currentItem;
            currentItem = currentItem->next;
        }
        return false;
    }
}

template<typename T>
T OULinkedList<T>::get() const {

    T result;
    if (first != NULL) {
        result = *first->data; // return first element
    }
    else {
        throw new ExceptionLinkedListAccess();
    }
    return result;
}

template<typename T>
OULink<T>* OULinkedList<T>::getFirstLink() {

    if (first == NULL) {
        throw new ExceptionLinkedListAccess();
    }

    return first;
}

template<typename T>
T OULinkedList<T>::find(const T* item) const {

    T foundItem;
    // list is empty, return false
    if (first == NULL) {
        throw new ExceptionLinkedListAccess();
    }
    // first data is identical to item, so return item
    else if (comparator->compare(*item, *first->data) == 0) {
        foundItem = *item;
    }
    else {
        // check the rest of the list
        OULink<T> *currentItem = first;

        while (currentItem != NULL) {

            if (comparator->compare(*currentItem->data, *item) == 0) {
                return *currentItem->data;
            }
            currentItem = currentItem->next;
        }

        // nothing found so throw exception
        throw new ExceptionLinkedListAccess();
    }
    return foundItem;
}

template<typename T>
unsigned long OULinkedList<T>::getSize() const {
    return size; // return size of list
}

template<typename T>
OULinkedListEnumerator<T> OULinkedList<T>::enumerator() const {
    OULinkedListEnumerator<T> listEnum(first);
    return listEnum;
}
#endif // !OU_LINKED_LIST
