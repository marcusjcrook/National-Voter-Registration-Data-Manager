#ifndef OU_LINKED_LIST_ENUMERATOR
#define OU_LINKED_LIST_ENUMERATOR

#include "Enumerator.h"
#include "OULink.h"
#include "Exceptions.h"

template <typename T>
class OULinkedListEnumerator : public Enumerator<T>
{
private:
    OULink<T>* current;
public:
    explicit OULinkedListEnumerator(OULink<T>* first);
    virtual ~OULinkedListEnumerator();
    bool hasNext() const;
    T next();
    T peek() const;
};

// constructor for OULinkedListEnumerator
template <typename T>
OULinkedListEnumerator<T>::OULinkedListEnumerator(OULink<T>* first) {
    current = first; // set current to first element
}

template <typename T>
OULinkedListEnumerator<T>::~OULinkedListEnumerator() = default;

// returns true if there is a next element
template <typename T>
bool OULinkedListEnumerator<T>::hasNext() const {
    // LinkedList has next element, return true
    return current != NULL;
}

 //returns copy of next element and advances to next position
 //throws ExceptionEnumerationBeyondEnd if no next item is available
template <typename T>
T OULinkedListEnumerator<T>::next() {

    if (current == NULL) {
        throw new ExceptionEnumerationBeyondEnd();
    }

    T item = *current->data; // return data for current
    current = current->next; // advance position
    return item;
}

// returns copy of next element without advancing position
// throws ExceptionEnumerationBeyondEnd if no next item is available
template<typename T>
T OULinkedListEnumerator<T>::peek() const {

    if (current == NULL) {
        throw new ExceptionEnumerationBeyondEnd();
    }
    T item = *current->data; // return data without advancing position
    return item;
}

#endif // !OU_LINKED_LIST_ENUMERATOR
