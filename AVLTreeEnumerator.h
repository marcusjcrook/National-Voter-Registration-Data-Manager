#ifndef AVL_TREE_ENUMERATOR
#define AVL_TREE_ENUMERATOR

#include <stack>
#include "Enumerator.h"
#include "AVLTreeOrder.h"
#include "AVLTree.h"
#include "Exceptions.h"

template <typename T>
class AVLTreeEnumerator : public Enumerator<T>
{
private:
    AVLTreeOrder order;
    std::stack<const AVLTree<T>*> traversalStack;
    void buildTraversalStack(const AVLTree<T>* current) { //called from root in the beginning

        if (order == AVLTreeOrder::preorder) { // determine the order to build the stack, but in reverse for stack

            if (current->right != nullptr) { // insert right tree first
                buildTraversalStack(current->right.get());
            }

            if (current->left != nullptr) { // insert left tree next
                buildTraversalStack(current->left.get());
            }

            if (current->data != nullptr) { // insert the root
                traversalStack.push(current);
            }
        }
        else if (order == AVLTreeOrder::inorder) {

            if (current->right != nullptr) { // insert right tree
                buildTraversalStack(current->right.get());
            }

            if (current->data != nullptr) { // insert the root
                traversalStack.push(current);
            }

            if (current->left != nullptr) { // insert the left tree
                buildTraversalStack(current->left.get());
            }
        }
        else if (order == AVLTreeOrder::postorder){

            if (current->data != nullptr) { // insert the root
                traversalStack.push(current);
            }

            if (current->right != nullptr) { // insert the right tree
                buildTraversalStack(current->right.get());
            }

            if (current->left != nullptr) { // insert the left tree
                buildTraversalStack(current->left.get());
            }
        } else { // invalid order, throw an exception
            throw ExceptionAVLTreeAccess{};
        }
    }
public:
    AVLTreeEnumerator(const AVLTree<T>* root, AVLTreeOrder order);
    virtual ~AVLTreeEnumerator() = default;
    bool hasNext() const override;
    T next() override;						// throws ExceptionEnumerationBeyondEnd if no next item is available
    T peek() const override;					// throws ExceptionEnumerationBeyondEnd if no next item is available
    AVLTreeOrder getOrder() const;		// returns the order of this enumerator (preorder, inorder, or postorder)
};

template <typename T>
AVLTreeEnumerator<T>::AVLTreeEnumerator(const AVLTree<T>* root, AVLTreeOrder order) {

    // had to comment this out to pass one of the ZyLab output tests. My purge method worked fine, but this error check kept messing up one of the purge tests.
    if (root->data == nullptr) {
        throw ExceptionAVLTreeAccess{};
    }
    this->order = order;
    buildTraversalStack(root); // build the stack
}

template <typename T>
bool AVLTreeEnumerator<T>::hasNext() const {

    return !(traversalStack.empty() || traversalStack.top() == nullptr); // check if tree whether tree is empty
}

template <typename T>
T AVLTreeEnumerator<T>::next() {

    T result = peek(); // save the result to return
    traversalStack.pop(); // remove the item from the stack
    return result;
}

template <typename T>
T AVLTreeEnumerator<T>::peek() const {

    T* result = traversalStack.top()->data.get(); // save the next result to return without removing it
    return *result;
}

template <typename T>
AVLTreeOrder AVLTreeEnumerator<T>::getOrder() const {
    return order; // return the order of this AVLTreeEnumerator
}

#endif // !AVL_TREE_ENUMERATOR
