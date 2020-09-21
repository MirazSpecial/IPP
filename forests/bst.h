#ifndef BST_H
#define BST_H

struct Node;

typedef struct Node* Tree;

struct Node {
    char* value;
    Tree subtree;
    Tree left, right;
};

Tree insert(Tree* treePointer, char* elem);
void removeval(Tree* treePointer, char* elem);
void printall(Tree nodePtr);
void freeall(Tree* treePointer);
Tree findnode(Tree nodePtr, char* elem);

#endif /* BST_H */
