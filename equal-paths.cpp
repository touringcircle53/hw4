#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
int nodeHeight(Node* root) {
    
    if (root == nullptr) {
        return 0;
    }

    int leftHeight = nodeHeight(root->left); // recurse to the bottom of the tree
    int rightHeight = nodeHeight(root->right); // for both

    if (leftHeight == -1 || rightHeight == -1) {
        return -1;
    }

    if (root->left != nullptr && root->right != nullptr) { // if they have both their left and right children
        if (leftHeight != rightHeight) { // and their heights are different;
            return -1;
        }
        return leftHeight + 1; // return the height at that node, and since left and right height are equal, we just choose 1
    }
    else if (root->left != nullptr) { // if the node only has left child
        return leftHeight + 1; 
    }
    else if (root->right != nullptr) { // if the node only has right child
        return rightHeight + 1;
    }
    else { // if the node is a leaf, we return a height of 1
        return 1;
    }
}

bool equalPaths(Node * root)
{
    // Add your code below
    return nodeHeight(root) != -1;

}

