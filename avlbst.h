#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void insertFix(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* node);
    void removeFix(AVLNode<Key, Value>* node, int8_t diff);
    void rotateLeft(AVLNode<Key, Value>* node);
    void rotateRight(AVLNode<Key, Value>* node);


};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO


    const Key& newKey = new_item.first;
    const Value& newVal = new_item.second;

    if (this->root_ == nullptr) {
        this->root_ = new AVLNode<Key, Value>(newKey, newVal, nullptr);
        return;
    }

    AVLNode<Key, Value>* current = static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key, Value>* parent = nullptr;

    while (current != nullptr) {
        parent = current;
        if (newKey < current->getKey()) {
            current = current->getLeft();
        }
        else if (newKey > current->getKey()) {
            current = current->getRight();
        }
        else {
            current->setValue(newVal);
            return;
        }
    }

    AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(newKey, newVal, parent);

    if (newKey < parent->getKey()) {
        parent->setLeft(newNode);
    }
    else {
        parent->setRight(newNode);
    }

    if (parent->getBalance() == -1 || parent->getBalance() == 1) {
        parent->setBalance(0);
    }
    else {
        if (parent->getLeft() == newNode) {
            parent->setBalance(-1);
        }
        else {
            parent->setBalance(1);
        }
        insertFix(parent, newNode);
    }
    
    
}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* node) {
    if (parent == nullptr || parent->getParent() == nullptr) {
        return;
    }

    AVLNode<Key, Value>* gpa = parent->getParent();

    if (gpa->getLeft() == parent) { // parent is the left child of grandparent
        gpa->updateBalance(-1);

        if (gpa->getBalance() == 0) {
            return;
        }
        else if (gpa->getBalance() == -1) {
            insertFix(gpa, parent);
        }
        else if (gpa->getBalance() == -2) { // left heavy therefore rotation needed
            if (parent->getLeft() == node) { // zig-zag right rotation
                rotateRight(gpa);
                parent->setBalance(0);
                gpa->setBalance(0);
            }
            else { // zig-zag left-right rotation
                rotateLeft(parent);
                rotateRight(gpa);

                if (node->getBalance() == -1) {
                    parent->setBalance(0);
                    gpa->setBalance(1);
                }
                else if (node->getBalance() == 0) {
                    parent->setBalance(0);
                    gpa->setBalance(0);
                }
                else {
                    parent->setBalance(-1);
                    gpa->setBalance(0);
                }
                node->setBalance(0);

            }
        }
    }
    else { // parent is the right child of grandparent
        gpa->updateBalance(1);

        if (gpa->getBalance() == 0) {
            return;
        }
        else if (gpa->getBalance() == 1) {
            insertFix(gpa, parent);
        }
        else if (gpa->getBalance() == 2) { // right heavy so needs rotation
            if (parent->getRight() == node) { // zig zag left rotation
                rotateLeft(gpa);
                parent->setBalance(0);
                gpa->setBalance(0);
            }
            else { // zig-zag right left rotation
                rotateRight(parent);
                rotateLeft(gpa);

                if (node->getBalance() == 1) {
                    parent->setBalance(0);
                    gpa->setBalance(-1);
                }
                else if (node->getBalance() == 0) {
                    parent->setBalance(0);
                    gpa->setBalance(0);
                }
                else {
                    parent->setBalance(1);
                    gpa->setBalance(0);
                }
                node->setBalance(0);
            }
        }
    }
}


/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
    

    AVLNode<Key, Value>* nodeToDel = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));

    if (nodeToDel == nullptr) {
        return;
    }

    if ((nodeToDel->getLeft() != nullptr) && (nodeToDel->getRight() != nullptr)) {
        AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(BinarySearchTree<Key, Value>::predecessor(nodeToDel));
        nodeSwap(nodeToDel, pred);
    }

    AVLNode<Key, Value>* parent = nodeToDel->getParent();
    AVLNode<Key, Value>* child = nullptr;
    int8_t diff = 0;

    if (nodeToDel->getLeft() != nullptr) {
        child = nodeToDel->getLeft();
    }
    else if (nodeToDel->getRight() != nullptr) {
        child = nodeToDel->getRight();
    }

    if (parent != nullptr) {
        if (parent->getLeft() == nodeToDel) {
            diff = 1;
        }
        else {
            diff = -1;
        }
    }

    if (parent == nullptr) {
        this->root_ = child;
    }
    else {
        if (parent->getLeft() == nodeToDel) {
            parent->setLeft(child);
        }
        else {
            parent->setRight(child);
        }
    }

    if (child != nullptr) {
        child->setParent(parent);
    }

    delete nodeToDel;

    removeFix(parent, diff);

}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* node, int8_t diff) {
    if (node == nullptr) {
        return;
    }

    AVLNode<Key, Value>* parent = node->getParent();
    int8_t nextDiff = 0;

    if (parent != nullptr) {
        if (parent->getLeft() == node) {
            nextDiff = 1;
        }
        else {
            nextDiff = -1;
        }
    }

    if (diff == -1) {
        if (node->getBalance() + diff == -2) {
            AVLNode<Key, Value>* tallerChild = node->getLeft();

            if (tallerChild->getBalance() == -1) {
                rotateRight(node);
                node->setBalance(0);
                tallerChild->setBalance(0);
                removeFix(parent, nextDiff);
            }
            else if (tallerChild->getBalance() == 0) {
                rotateRight(node);
                node->setBalance(-1);
                tallerChild->setBalance(1);
            }
            else {
                AVLNode<Key, Value>* gchild = tallerChild->getRight();
                rotateLeft(tallerChild);
                rotateRight(node);

                if (gchild->getBalance() == 1) {
                    node->setBalance(0);
                    tallerChild->setBalance(-1);
                }
                else if (gchild->getBalance() == 0) {
                    node->setBalance(0);
                    tallerChild->setBalance(0);
                }
                else {
                    node->setBalance(1);
                    tallerChild->setBalance(0);
                }
                gchild->setBalance(0);
                removeFix(parent, nextDiff);
            }
        }
        else if (node->getBalance() + diff == -1) {
            node->setBalance(-1);
        }
        else {
            node->setBalance(0);
            removeFix(parent, nextDiff);
        }
    }
    else {
        if (node->getBalance() + diff == 2) {
            AVLNode<Key, Value>* tallerChild = node->getRight();

            if (tallerChild->getBalance() == 1) {
                rotateLeft(node);
                node->setBalance(0);
                tallerChild->setBalance(0);
                removeFix(parent, nextDiff);
            }
            else if (tallerChild->getBalance() == 0) {
                rotateLeft(node);
                node->setBalance(1);
                tallerChild->setBalance(-1);
            }
            else {
                AVLNode<Key, Value>* gchild = tallerChild->getLeft();
                rotateRight(tallerChild);
                rotateLeft(node);

                if (gchild->getBalance() == -1) {
                    node->setBalance(0);
                    tallerChild->setBalance(1);
                }
                else if (gchild->getBalance() == 0) {
                    node->setBalance(0);
                    tallerChild->setBalance(0);
                }
                else {
                    node->setBalance(-1);
                    tallerChild->setBalance(0);
                }
                gchild->setBalance(0);
                removeFix(parent, nextDiff);
            }
        }
        else if (node->getBalance() + diff == 1) {
            node->setBalance(1);
        }
        else {
            node->setBalance(0);
            removeFix(parent, nextDiff);
        }
    }
}


template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* node) {
    AVLNode<Key, Value>* rightChild = node->getRight();
    AVLNode<Key, Value>* parent = node->getParent();

    node->setRight(rightChild->getLeft());
    if (rightChild->getLeft() != nullptr) {
        rightChild->getLeft()->setParent(node);
    }

    rightChild->setLeft(node);
    node->setParent(rightChild);
    rightChild->setParent(parent);

    if (parent == nullptr) {
        this->root_ = rightChild;
    }
    else if (parent->getLeft() == node) {
        parent->setLeft(rightChild);
    }
    else {
        parent->setRight(rightChild);
    }
    
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* node) {
    AVLNode<Key, Value>* leftChild = node->getLeft();
    AVLNode<Key, Value>* parent = node->getParent();

    node->setLeft(leftChild->getRight());
    if (leftChild->getRight() != nullptr) {
        leftChild->getRight()->setParent(node);
    }

    leftChild->setRight(node);
    node->setParent(leftChild);
    leftChild->setParent(parent);

    if (parent == nullptr) {
        this->root_ = leftChild;
    }
    else if (parent->getLeft() == node) {
        parent->setLeft(leftChild);
    }
    else {
        parent->setRight(leftChild);
    }
}

#endif
