#ifndef OU_LINK_H
#define OU_LINK_H

#include <memory>
#include <new>
#include "Exceptions.h"

template <typename T>
class OULink {
    template <typename>
    friend class OULinkedList;
    template <typename>
    friend class OULinkedListEnumerator;
private:
    std::unique_ptr<T> data;								// owned data item
    std::unique_ptr<OULink> next;							// owned next link
public:
    explicit OULink(const T* item);
    virtual ~OULink() = default;
};

// constructor for OULink
template <typename T>
OULink<T>::OULink(const T* item)
    : data(new (std::nothrow) T(*item)) {
    if (!data) {
        throw ExceptionMemoryNotAvailable{};
    }
}
#endif // !OU_LINK_H