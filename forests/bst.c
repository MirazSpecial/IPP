#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "bst.h"

Tree insert(Tree* treePointer, char* elem) {
    // inserts node to BST, return pointer to it
    Tree ourTree = *treePointer;
    if (ourTree == NULL) { // add node here
        ourTree = (Tree)malloc(sizeof(struct Node));
        size_t word_size = strlen(elem);
        char* new_word = (char*)malloc(word_size + 1); // +1 becouse \0

        if (ourTree == NULL || new_word == NULL) exit(1); // failed to malloc

        new_word = strcpy(new_word, elem);
        ourTree->value = new_word;
        ourTree->subtree = ourTree->left = ourTree->right = NULL;
        *treePointer = ourTree;
        return ourTree; 
    }
    else if (strcmp(ourTree->value, elem) > 0) 
        return insert(&ourTree->left, elem);
    else if (strcmp(ourTree->value, elem) < 0)
        return insert(&ourTree->right, elem);
    else // value already in BSTtree
        return ourTree;
}

static Tree* findmin(Tree* treePointer) {
    // finds node with minimum value in non-empty bst tree 
    // and returns its parents pointer on it
    Tree ourTree = *treePointer;
    if (ourTree->left == NULL)
        return treePointer;
    else
        return findmin(&ourTree->left);
}

static void removemin(Tree* treePointer) {
    // removes node with minimum value - **treePointer
    Tree ourTree = *treePointer;
    *treePointer = ourTree->right;
    free(ourTree);
}

void printall(Tree nodePtr) {
    // prints values of all nodes 
    if (nodePtr != NULL) {
        printall(nodePtr->left);
        printf("%s\n", nodePtr->value);
        printall(nodePtr->right);
    }
}

void freeall(Tree* treePointer) {
    // frees bst tree and all -> subtrees of its nodes
    Tree ourTree = *treePointer;
    if (ourTree != NULL) {
        freeall(&ourTree->left);
        freeall(&ourTree->right);
        freeall(&ourTree->subtree);
        free(ourTree->value);
        free(ourTree);
        *treePointer = NULL;
    }
}

void removeval(Tree* treePointer, char* elem) { 
    // removes node from bst and frees its ->subtree
    Tree ourTree = *treePointer;
    if (ourTree != NULL) {
        if (strcmp(ourTree->value, elem) > 0)
            removeval(&ourTree->left, elem);
        else if (strcmp(ourTree->value, elem) < 0)
            removeval(&ourTree->right, elem);
        else {  // remove this node
            freeall(&ourTree->subtree);
            free(ourTree->value);
            if (ourTree->right == NULL) {
                *treePointer = ourTree->left;
                free(ourTree);
            }
            else { // when ourTree->right is non-empty
                Tree* minnode = findmin(&ourTree->right);
                ourTree->value = (*minnode)->value;
                ourTree->subtree = (*minnode)->subtree;
                removemin(minnode);
            }
        }
    }
}

Tree findnode(Tree nodePtr, char* elem) {
    // return pointer to node with ->value = elem, or NULL 
    if (nodePtr == NULL) 
        return NULL;
    else if (strcmp(nodePtr->value, elem) > 0) 
        return findnode(nodePtr->left, elem);
    else if (strcmp(nodePtr->value, elem) < 0)
        return findnode(nodePtr->right, elem);
    else
        return nodePtr;   
}