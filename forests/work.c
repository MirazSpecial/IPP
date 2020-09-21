#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "bst.h"
#include "work.h"

static void add(Tree* treePointer, command* curr_comm) {
    Tree researchtree = *treePointer;
    researchtree = insert(treePointer, curr_comm->words[0]);
    if (curr_comm->words[1] == NULL) return; // ADD a
    researchtree = insert(&researchtree->subtree, curr_comm->words[1]);
    if (curr_comm->words[2] == NULL) return; // ADD a b
    researchtree = insert(&researchtree->subtree, curr_comm->words[2]);
    return; //ADD a b c
}

static void del(Tree* treePointer, command* curr_comm) {
    Tree researchtree = *treePointer;
    if (curr_comm->words[0] == NULL) { // DEL
        freeall(treePointer);
        return;
    }
    if (curr_comm->words[1] == NULL) { // DEL a
        removeval(treePointer, curr_comm->words[0]);
        return;
    }
    researchtree = findnode(researchtree, curr_comm->words[0]);
    if (researchtree == NULL) 
        return;
    if (curr_comm->words[2] == NULL) { // DEL a b
        removeval(&researchtree->subtree, curr_comm->words[1]);
        return;
    }
    researchtree = findnode(researchtree->subtree, curr_comm->words[1]);
    if (researchtree == NULL)
        return;
    removeval(&researchtree->subtree, curr_comm->words[2]); // DEL a b c
}

static void print(Tree ourTree, command* curr_comm) {
    if (curr_comm->words[0] == NULL) {
        printall(ourTree);
        return; // PRINT
    }
    ourTree = findnode(ourTree, curr_comm->words[0]);
    if (ourTree == NULL) return; // PRINT a, but there is no a
    ourTree = ourTree->subtree;
    if (curr_comm->words[1] == NULL) {
        printall(ourTree);
        return; // PRINT a
    }
    ourTree = findnode(ourTree, curr_comm->words[1]);
    if (ourTree == NULL) return; // PRINT a b, but there is no a b
    ourTree = ourTree->subtree;
    printall(ourTree);
    return; // PRINT a b
}

static bool check_all_subtrees(Tree nodePtr, char* elem) {
    // check if elem in ->subtree of any node 
    if (nodePtr == NULL)
        return false;
    if (findnode(nodePtr->subtree, elem) != NULL ||
        check_all_subtrees(nodePtr->left, elem) ||
        check_all_subtrees(nodePtr->right, elem)) return true;
    return false;
}

static bool check_all_subtrees_subtrees(Tree nodePtr, char* elem) {
    // check if elem in ->subtree of any node in ->subtree of any node
    if (nodePtr == NULL)
        return false;
    if (check_all_subtrees(nodePtr->subtree, elem) ||
        check_all_subtrees_subtrees(nodePtr->left, elem) ||
        check_all_subtrees_subtrees(nodePtr->right, elem)) return true;
    return false;
}

static bool check_check_all_subtrees (Tree nodePtr, char* elem1, char* elem2) {
    // check if elem2 in ->subtree of node elem1 in ->subtree of any node
    if (nodePtr == NULL)
        return false;
    Tree ourTree = findnode(nodePtr->subtree, elem1);
    if ((ourTree != NULL && findnode(ourTree->subtree, elem2) != NULL) ||
        check_check_all_subtrees(nodePtr->left, elem1, elem2) ||
        check_check_all_subtrees(nodePtr->right, elem1, elem2)) return true;
    return false;
}

static bool check(Tree* treePointer, command* curr_comm) {
    Tree ourTree = *treePointer;
    char* word1 = curr_comm->words[0];
    char* word2 = curr_comm->words[1];
    char* word3 = curr_comm->words[2];

    if (!strcmp(word1, "*") && !strcmp(word2, "*")) // case CHECK * * a
        return check_all_subtrees_subtrees(ourTree, word3);
    if (!strcmp(word1, "*") && word3 == NULL) // case CHECK * a
        return check_all_subtrees(ourTree, word2);
    if (!strcmp(word1, "*")) // case CHECK * a b
        return check_check_all_subtrees(ourTree, word2, word3);

    ourTree = findnode(ourTree, word1);
    
    if (ourTree == NULL) return false; // case CHECK a ..., but no a
    if (word2 == NULL) // case CHECK a
        return true;
    if (!strcmp(word2, "*")) // case CHECK a * b
        return check_all_subtrees(ourTree->subtree, word3);

    ourTree = findnode(ourTree->subtree, word2);
    
    if (ourTree == NULL) return false; // case CHECK a b ..., but no a b
    if (word3 == NULL) // case CHECK a b
        return true;
    if (findnode(ourTree->subtree, word3) != NULL) // case CHECK a b c
        return true;
    return false;
}

void operation_handle(Tree* treePointer, command* currant_command) {
    // choosing function adequately to command 
    switch(currant_command->oprtype) {
        case 'A': // command ADD
            add(treePointer, currant_command);
            printf("OK\n");
            break;
        case 'D': // command DEL
            del(treePointer, currant_command);
            printf("OK\n");
            break;
        case 'C': // command CHECK
            printf(check(treePointer, currant_command) ? "YES\n" : "NO\n");
            break;
        case 'P': // command PRINT
            print(*treePointer, currant_command);
            break;
    }
}
