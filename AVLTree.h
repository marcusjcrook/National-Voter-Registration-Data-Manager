#ifndef AVL_TREE
#define AVL_TREE

#include "Comparator.h"
#include <cstdlib>
#include "Exceptions.h"
#include <memory>
#include <utility>

template <typename T>
class AVLTree {
    template <typename U>
    friend class AVLTreeEnumerator;
private:
    Comparator<T>* comparator = nullptr;				// used to determine tree order and item equality
    unsigned long size = 0; 						// actual number of items currently in tree
    std::unique_ptr<T> data;							// owned data at this node
    int diff = 0;									// height of right minus height of left
    std::unique_ptr<AVLTree<T>> left;					// owned left subtree
    std::unique_ptr<AVLTree<T>> right;					// owned right subtree
    void zig();										// right rotation
    void zag();										// left rotation
    void zigzag();									// left rotation on left subtree, followed by right rotation
    void zagzig();									// right rotation on right subtree, followed by left rotation
    void rebalance();								// check for and re-balance this node, if needed

public:
    explicit AVLTree(Comparator<T>* comparator);	// creates empty linked tree with comparator
    AVLTree(const AVLTree& other);
    AVLTree& operator=(const AVLTree& other);
    AVLTree(AVLTree&& other) noexcept = default;
    AVLTree& operator=(AVLTree&& other) noexcept = default;
    virtual ~AVLTree() = default;
    void swap(AVLTree& other) noexcept;

    // if an equivalent item is not already present, insert item in order and return true
    // if an equivalent item is already present, leave tree unchanged and return false
    bool insert(const T* item);

    // if an equivalent item is already present, replace item and return true
    // if an equivalent item is not already present, leave tree unchanged and return false
    bool replace(const T* item);

    // if an equivalent item is already present, remove item and return true
    // if an equivalent item is not already present, leave tree unchanged and return false
    bool remove(const T* item);

    // if an equivalent item is present, return a copy of the item
    // if an equivalent item is not present, throw ExceptionAVLTreeAccess
    T find(const T* item) const;

    // returns the current number of items in the tree
    unsigned long getSize() const;
};

template<typename T>
AVLTree<T>::AVLTree(Comparator<T>* comparator) {
    this->comparator = comparator;
}

template<typename T>
AVLTree<T>::AVLTree(const AVLTree& other)
    : comparator(other.comparator),
      size(other.size),
      data(other.data ? std::make_unique<T>(*other.data) : nullptr),
      diff(other.diff),
      left(other.left ? std::make_unique<AVLTree<T>>(*other.left) : nullptr),
      right(other.right ? std::make_unique<AVLTree<T>>(*other.right) : nullptr) {
}

template<typename T>
AVLTree<T>& AVLTree<T>::operator=(const AVLTree& other) {
    if (this != &other) {
        AVLTree copy(other);
        swap(copy);
    }
    return *this;
}

template<typename T>
void AVLTree<T>::swap(AVLTree& other) noexcept {
    using std::swap;
    swap(comparator, other.comparator);
    swap(size, other.size);
    swap(data, other.data);
    swap(diff, other.diff);
    swap(left, other.left);
    swap(right, other.right);
}

template<typename T>
bool AVLTree<T>::insert(const T* item) { // todo: need to check for available memory
    bool result = false;
    if (data == nullptr) { // no data in tree
        data = std::make_unique<T>(*item); // create new item
        result = true; // return true
    } else {
        if (comparator->compare(*item, *data) < 0) { // item is less than data, so attempt to insert in left tree
            if (left == nullptr) { // no left tree yet, so create it and add the element
                left = std::make_unique<AVLTree<T>>(comparator);
                left->data = std::make_unique<T>(*item);
                result = true;
                diff--; // decrease diff
            } else { // left tree not nullptr, call insert recursively
                int leftDiff = left->diff; // store the left diff for comparison after attempted insert
                bool leftResult = left->insert(item); // was the item inserted?
                if (leftResult) {
                    result = true;
                }
                if (leftDiff != left->diff) { // left diff has changed from 0 to 1 or -1, so decrease diff
                    if (leftDiff == 0 && (left->diff == 1 || left->diff == -1)) {
                        diff--;
                    }
                }
            }
        } else if (comparator->compare(*item, *data) > 0) { // item is greater than item stored at data, so attempt to insert in right tree
            if (right == nullptr) { // no right tree yet, create it and insert the item
                right = std::make_unique<AVLTree<T>>(comparator);
                right->data = std::make_unique<T>(*item);
                result = true;
                diff++;
            } else { // right not nullptr, so same drill as above
                int rightDiff = right->diff;
                bool rightResult = right->insert(item);
                if (rightResult) {
                    result = true;
                }
                if (rightDiff != right->diff) { // diff changed from 0 to 1 or -1, so increase diff
                    if (rightDiff == 0 && (right->diff == 1 || right->diff == -1)) {
                        diff++;
                    }
                }
            }
        } else {
            result = false;
        }
    }

    if (result) {
        rebalance(); // rebalance if a new item has been inserted
        size++;
    }

    return result;
}

template<typename T>
bool AVLTree<T>::replace(const T* item) {

    bool result = false;
    if (comparator->compare(*item, *data) == 0) { // item to replace is equal to data, so replace it
        data = std::make_unique<T>(*item);
        result = true;
    } else {
        if (comparator->compare(*item, *data) < 0) { // item is less than data, so call replace on left
            if (left == nullptr) { // left is nullptr, so no greater item
                result = false;
            }
            else {
                bool leftResult = left->replace(item); // left is not nullptr, call replace on left
                if (leftResult) {
                    result = true;
                }
            }
        } else if (comparator->compare(*item, *data) > 0) { // item is greater than data, so same drill as above
            if (right == nullptr) {
                result = false;
            } else {
                bool rightResult = right->replace(item);
                if (rightResult) {
                    result = true;
                }
            }
        }
    }

    return result;
}

template<typename T>
bool AVLTree<T>::remove(const T* item) {
    bool result = false;

    if (data == nullptr) { // data is nullptr, so nothing to remove from
        return false;
    }

    if (comparator->compare(*item, *data) == 0) { // item is equal to data
        if (left == nullptr && right == nullptr) { // if left and right are both nullptr, set data to nullptr
            data.reset();
            result = true;
        } else if (left != nullptr && right == nullptr) { // if right is nullptr, but left is not, set data to left data
            data = std::make_unique<T>(*left->data);
            result = true;
            bool leftResult = left->remove(left->data.get());

            if (leftResult) {
                result = true;
            }
            if(left->data == nullptr) { // if left data is nullptr, item was deleted, so delete and set nullptr
                left.reset();
                diff++;
            }
        } else if (left == nullptr && right != nullptr){ // if left is nullptr, but right is not, set data to right data

            data = std::make_unique<T>(*right->data);
            result = true;
            bool rightResult = right->remove(right->data.get());

            if (rightResult) {
                result = true;
            }

            if (right->data == nullptr) { // if right data is nullptr, it was removed, so delete it and set the right to nullptr
                right.reset();
                diff--;
            }
        } else { // neither side is nullptr
            if (right->left == nullptr) { // if right->left is nullptr, set the data to the item on the right
                data = std::make_unique<T>(*right->data);
                result = true;
                bool rightResult = right->remove(right->data.get());

                if (rightResult) {
                    result = true;
                }

                if (right->data == nullptr) { // right data is NUL, so delete the tree and set it to nullptr
                    right.reset();
                    diff--;
                }

            } else if (right->left != nullptr) { // right->left is not nullptr
                if (right->left->left == nullptr) { // right->left->left is nullptr, so set data to right->left
                    int leftRightDiff = right->left->diff; // keep track of the diff
                    data = std::make_unique<T>(*right->left->data);
                    bool rightLeftResult = right->left->remove(right->left->data.get());

                    if (leftRightDiff == 1 && right->left->diff == 0) { // relevant change in diff, so increase diff
                        right->diff++;
                    }

                    if (rightLeftResult) {
                        result = true;
                    }

                    if (right->left->data == nullptr) { // if left->right is nullptr, delete it and set to nullptr
                        right->left.reset();
                        if (right->right != nullptr) { // if right->right is not nullptr, need to increase right diff and rebalance the right tree
                            right->diff++;
                            right->rebalance();
                        }
                        if (right->right == nullptr) { // right->right is nullptr, so decrease diff
                            diff--;
                        }
                    }

                } else { // right->left->left is not nullptr, so need to find left-most tree in the right side

                    AVLTree<T>* temp = right->left.get();
                    while (temp->left != nullptr) { // change temp until the left-most item is found
                        temp = temp->left.get();
                    }
                    data = std::make_unique<T>(*temp->data);
                    bool rightResult = right->remove(temp->data.get()); // remove the data from the right tree

                    if (rightResult) {
                        result = true;
                    }

                    if (right->data == nullptr) { // if right data is nullptr, need to increment diff
                        right.reset();
                        diff++;
                    }
                }
            }
        }
    } else if (comparator->compare(*item, *data) < 0) { // the item is less than data so call remove on left subtree
        if (left != nullptr) {
            int leftDiff = left->diff; // keep track of diff
            bool leftResult = left->remove(item); // call remove on left subtree

            if ((leftDiff == 1 || leftDiff == -1) && left->diff == 0) {
                diff++;
            }

            if (leftResult) {
                result = true;
            }

            if (left->data == nullptr) { // if left data is nullptr, need to increase diff
                left.reset();
                diff++;
            }
        }

    } else if (comparator->compare(*item, *data) > 0) { // if item is greater than data, need to call remove on right subtree

        if (right != nullptr) {
            int rightDiff = right->diff; // keep track of changes in diff
            bool rightResult = right->remove(item);

            if ((rightDiff == 1 || rightDiff == -1) && right->diff == 0) {
                diff--;
            }

            if (rightResult) {
                result = true;
            }

            if (right->data == nullptr) { // if right data is nullptr, delete it and set it to nullptr
                right.reset();
                diff--;
            }
        }
    }

    if (result) {
        rebalance(); // rebalance if an item was removed
        size--;
    }

    return result;
}

template<typename T>
T AVLTree<T>::find(const T* item) const {

    if (data == nullptr) { // tree is empty, so throw an exception
        throw ExceptionAVLTreeAccess{};
    }

    T result;

    if (comparator->compare(*item, *data) == 0) { // item is equal to data so return the item
        result = *data;
    }
    else if (comparator->compare(*item, *data) < 0) { // item is less than data, so call find on left subtree
            if (left == nullptr) { // left is nullptr, so throw exception
                throw ExceptionAVLTreeAccess{};
            } else {
                result = left->find(item);
            }

    } else {
            if (right == nullptr) { // right is nullptr, so throw an exception
                throw ExceptionAVLTreeAccess{};
            } else {
                result = right->find(item);
            }
    }

    return result;
}

template<typename T>
unsigned long AVLTree<T>::getSize() const { // return the size of the tree
    return size;
}

template<typename T>
void AVLTree<T>::zig()
{
    if (left == nullptr) {  // no left, no way to zig
        return;
    }
    // keep track of diff of node and left for changes
    int gdiff = diff;
    int pdiff = left->diff;

    // modify the tree
    auto olnr = std::move(left);  // olnr is "old left, new right"
    left = std::move(olnr->left);
    olnr->left = std::move(olnr->right);
    olnr->right = std::move(right);
    right = std::move(olnr);

    // note that the modifications kept the node itself in place, so we need to swap its data with its olnr's
    data.swap(right->data);

    // update the diff fields for node and new right
    if (pdiff < 0) {  // if imbalance was outside left heavy (left-left violation)
        diff = gdiff + 2;
        right->diff = gdiff - pdiff + 1;
    }
    else {  // otherwise imbalance was inside left heavy (left-right violation)
        diff = pdiff + 1;
        right->diff = gdiff + 1;
    }
}

template<typename T>
void AVLTree<T>::zag()
{
    if (right == nullptr) {  // no right, no way to zig
        return;
    }
    // keep track of diff of node and right for changes
    int gdiff = diff;
    int pdiff = right->diff;

    // modify the tree
    auto ornl = std::move(right);  // ornl is "old right, new left"
    right = std::move(ornl->right);
    ornl->right = std::move(ornl->left);
    ornl->left = std::move(left);
    left = std::move(ornl);

    // note that the modifications kept the node itself in place, so we need to swap its data with its ornl's
    data.swap(left->data);

    // update the diff fields for node and new left
    if (pdiff > 0) {  // if imbalance was outside right heavy (right-right violation)
        diff = gdiff - 2;
        left->diff = gdiff - pdiff - 1;
    }
    else {  // otherwise imbalance was inside right heavy (right-left violation)
        diff = pdiff - 1;
        left->diff = gdiff - 1;
    }
}

template<typename T>
void AVLTree<T>::zigzag() {
    left->zag(); // call zag on left subtree
    zig(); // call zig
}

template<typename T>
void AVLTree<T>::zagzig() {
    right->zig(); // call zig on right subtree
    zag(); // call zag
}

template<typename T>
void AVLTree<T>::rebalance() {
    if (diff == 2) { // diff is 2, so rebalance
        if (right->diff == -1) { // if right->diff is -1, need to double rotate
            zagzig();
        } else { // no double rotation
            zag();
        }
    }
    else if (diff == -2) { // diff is -2, so rebalance
        if (left->diff == 1) { // if left->diff is 1, need to double rotate
            zigzag();
        } else // no double rotation
            zig();
    }
}

#endif // !AVL_TREE
