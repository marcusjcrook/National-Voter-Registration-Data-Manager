#ifndef AVL_TREE
#define AVL_TREE

#include "Comparator.h"
#include <cstdlib>
#include "Exceptions.h"

template <typename T>
class AVLTree {
    template <typename U>
    friend class AVLTreeEnumerator;
private:
    Comparator<T>* comparator = NULL;				// used to determine tree order and item equality
    unsigned long size = 0; 						// actual number of items currently in tree
    T* data = NULL;									// pointer to data at this node
    int diff = 0;									// height of right minus height of left
    AVLTree<T>* left = NULL;						// pointer to left subtree
    AVLTree<T>* right = NULL;						// pointer to right subtree
    void zig();										// right rotation
    void zag();										// left rotation
    void zigzag();									// left rotation on left subtree, followed by right rotation
    void zagzig();									// right rotation on right subtree, followed by left rotation
    void rebalance();								// check for and re-balance this node, if needed

public:
    explicit AVLTree(Comparator<T>* comparator);	// creates empty linked tree with comparator
    virtual ~AVLTree();								// deletes all links and their data items

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
    // if an equivalent item is not present, throw a new ExceptionAVLTreeAccess
    T find(const T* item) const;

    // returns the current number of items in the tree
    unsigned long getSize() const;
};

template<typename T>
AVLTree<T>::AVLTree(Comparator<T>* comparator) {
    this->comparator = comparator;
}

template<typename T>
AVLTree<T>::~AVLTree() {
    if (data != NULL) { // delete the data in the tree
        delete data; // delete the data
        data = NULL; // set it to NULL
    }

    if (left != NULL) { // delete left tree
        delete left;
        left = NULL;
    }

    if (right != NULL) { // delete right tree
        delete right;
        right = NULL;
    }
}

template<typename T>
bool AVLTree<T>::insert(const T* item) { // todo: need to check for available memory
    bool result = false;
    if (data == NULL) { // no data in tree
        data = new T(*item); // create new item
        result = true; // return true
    } else {
        if (comparator->compare(*item, *data) < 0) { // item is less than data, so attempt to insert in left tree
            if (left == NULL) { // no left tree yet, so create it and add the element
                left = new AVLTree<T>(comparator);
                left->data = new T(*item);
                result = true;
                diff--; // decrease diff
            } else { // left tree not NULL, call insert recursively
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
            if (right == NULL) { // no right tree yet, create it and insert the item
                right = new AVLTree<T>(comparator);
                right->data = new T(*item);
                result = true;
                diff++;
            } else { // right not NULL, so same drill as above
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
        delete data;
        data = new T(*item);
        result = true;
    } else {
        if (comparator->compare(*item, *data) < 0) { // item is less than data, so call replace on left
            if (left == NULL) { // left is NULL, so no greater item
                result = false;
            }
            else {
                bool leftResult = left->replace(item); // left is not NULL, call replace on left
                if (leftResult) {
                    result = true;
                }
            }
        } else if (comparator->compare(*item, *data) > 0) { // item is greater than data, so same drill as above
            if (right == NULL) {
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

    if (data == NULL) { // data is NULL, so nothing to remove from
        return false;
    }

    if (comparator->compare(*item, *data) == 0) { // item is equal to data
        if (left == NULL && right == NULL) { // if left and right are both NULL, set data to NULL
            delete data;
            data = NULL;
            result = true;
        } else if (left != NULL && right == NULL) { // if right is NULL, but left is not, set data to left data
            delete data;
            data = new T(*left->data);
            result = true;
            bool leftResult = left->remove(left->data);

            if (leftResult) {
                result = true;
            }
            if(left->data == NULL) { // if left data is NULL, item was deleted, so delete and set NULL
                delete left;
                left = NULL;
                diff++;
            }
        } else if (left == NULL && right != NULL){ // if left is NULL, but right is not, set data to right data

            delete data;
            data = new T(*right->data);
            result = true;
            bool rightResult = right->remove(right->data);

            if (rightResult) {
                result = true;
            }

            if (right->data == NULL) { // if right data is NULL, it was removed, so delete it and set the right to NULL
                delete right;
                right = NULL;
                diff--;
            }
        } else { // neither side is NULL
            if (right->left == NULL) { // if right->left is NULL, set the data to the item on the right
                delete data;
                data = new T(*right->data);
                result = true;
                bool rightResult = right->remove(right->data);

                if (rightResult) {
                    result = true;
                }

                if (right->data == NULL) { // right data is NUL, so delete the tree and set it to NULL
                    delete right;
                    right = NULL;
                    diff--;
                }

            } else if (right->left != NULL) { // right->left is not NULL
                if (right->left->left == NULL) { // right->left->left is NULL, so set data to right->left
                    delete data;
                    int leftRightDiff = right->left->diff; // keep track of the diff
                    data = new T(*right->left->data);
                    bool rightLeftResult = right->left->remove(right->left->data);

                    if (leftRightDiff == 1 && right->left->diff == 0) { // relevant change in diff, so increase diff
                        right->diff++;
                    }

                    if (rightLeftResult) {
                        result = true;
                    }

                    if (right->left->data == NULL) { // if left->right is NULL, delete it and set to NULL
                        delete right->left;
                        right->left = NULL;
                        if (right->right != NULL) { // if right->right is not NULL, need to increase right diff and rebalance the right tree
                            right->diff++;
                            right->rebalance();
                        }
                        if (right->right == NULL) { // right->right is NULL, so decrease diff
                            diff--;
                        }
                    }

                } else { // right->left->left is not NULL, so need to find left-most tree in the right side

                    AVLTree<T>* temp = right->left;
                    while (temp->left != NULL) { // change temp until the left-most item is found
                        temp = temp->left;
                    }
                    delete data;
                    data = new T(*temp->data);
                    bool rightResult = right->remove(temp->data); // remove the data from the right tree

                    if (rightResult) {
                        result = true;
                    }

                    if (right->data == NULL) { // if right data is NULL, need to increment diff
                        delete right;
                        right = NULL;
                        diff++;
                    }
                }
            }
        }
    } else if (comparator->compare(*item, *data) < 0) { // the item is less than data so call remove on left subtree
        if (left != NULL) {
            int leftDiff = left->diff; // keep track of diff
            bool leftResult = left->remove(item); // call remove on left subtree

            if ((leftDiff == 1 || leftDiff == -1) && left->diff == 0) {
                diff++;
            }

            if (leftResult) {
                result = true;
            }

            if (left->data == NULL) { // if left data is NULL, need to increase diff
                delete left;
                left = NULL;
                diff++;
            }
        }

    } else if (comparator->compare(*item, *data) > 0) { // if item is greater than data, need to call remove on right subtree

        if (right != NULL) {
            int rightDiff = right->diff; // keep track of changes in diff
            bool rightResult = right->remove(item);

            if ((rightDiff == 1 || rightDiff == -1) && right->diff == 0) {
                diff--;
            }

            if (rightResult) {
                result = true;
            }

            if (right->data == NULL) { // if right data is NULL, delete it and set it to NULL
                delete right;
                right = NULL;
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

    if (data == NULL) { // tree is empty, so throw an exception
        throw new ExceptionAVLTreeAccess();
    }

    T result;

    if (comparator->compare(*item, *data) == 0) { // item is equal to data so return the item
        result = *data;
    }
    else if (comparator->compare(*item, *data) < 0) { // item is less than data, so call find on left subtree
            if (left == NULL) { // left is NULL, so throw exception
                throw new ExceptionAVLTreeAccess();
            } else {
                result = left->find(item);
            }

    } else {
            if (right == NULL) { // right is NULL, so throw an exception
                throw new ExceptionAVLTreeAccess();
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
    if (left == NULL) {  // no left, no way to zig
        return;
    }
    // keep track of diff of node and left for changes
    int gdiff = diff;
    int pdiff = left->diff;

    // modify the tree
    AVLTree<T>* olnr = left;  // olnr is "old left, new right"
    left = olnr->left;
    olnr->left = olnr->right;
    olnr->right = right;
    right = olnr;

    // note that the modifications kept the node itself in place, so we need to swap its data with its olnr's
    T* tempData = data;
    data = olnr->data;
    olnr->data = tempData;

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
    if (right == NULL) {  // no right, no way to zig
        return;
    }
    // keep track of diff of node and right for changes
    int gdiff = diff;
    int pdiff = right->diff;

    // modify the tree
    AVLTree<T>* ornl = right;  // ornl is "old right, new left"
    right = ornl->right;
    ornl->right = ornl->left;
    ornl->left = left;
    left = ornl;

    // note that the modifications kept the node itself in place, so we need to swap its data with its ornl's
    T* tempData = data;
    data = ornl->data;
    ornl->data = tempData;

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
