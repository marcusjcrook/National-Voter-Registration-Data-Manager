#ifndef OU_LINK_H
#define OU_LINK_H

#include <cstdlib>
#include <iostream>

template <typename T>
class OULink {
    template <typename>
    friend class OULinkedList;
    template <typename>
    friend class OULinkedListEnumerator;
private:
    T* data = nullptr;										// pointer to data item of any type
    OULink* next = nullptr;								// pointer to next link
public:
    explicit OULink(const T* item);
    virtual ~OULink();
};

// constructor for OULink
template <typename T>
OULink<T>::OULink(const T* item) {
    data = new T(*item);
}

// destructor for OULink
template<typename T>
OULink<T>::~OULink() {
    delete data; // delete data at first
    data = nullptr;
    delete next; // delete rest of list
    next = nullptr;

}
#endif // !OU_LINK_H